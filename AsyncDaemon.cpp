#include "AsyncDaemon.h"
#include <syscall.h>
#include <array>
#include <boost/algorithm/string.hpp>

using namespace std;

AsyncDaemon::Transaction::Transaction(
	HTTPServer::request const& request,
	int id)
: m_id(id)
{
	logMsg("transaction %d constructor", m_id);

	// determine the content length
	for (auto& h : request.headers)
	{
		if (boost::iequals(h.name, "content-length"))
		{
			m_content_length = stoul(h.value, nullptr, 10);
			break;
		}
	}

	// learn more about the request
	string ip_addr = source(request);
	string uri = destination(request);

	logMsg("received %zu byte request for %s from %s", m_content_length, uri.c_str(), ip_addr.c_str());
}

AsyncDaemon::Transaction::~Transaction()
{
	logMsg("transaction %d destructor", m_id);
}

void AsyncDaemon::Transaction::asyncRead(HTTPServer::connection_ptr connection)
{
	connection->read(ref(*this));
	logMsg("invoked asynchronous read");
}

void AsyncDaemon::Transaction::operator()(
	HTTPServer::connection::input_range r,
	boost::system::error_code error,
	size_t n,
	HTTPServer::connection_ptr connection)
{
	logMsg("begin asynchronous read callback on thread %lu", syscall(SYS_gettid));
	if (!error)
	{
		m_request_body.append(r.begin(), n);
		logMsg("read %zu bytes", n);

		if (m_request_body.length() < m_content_length)
		{
			asyncRead(connection);
		}
		else
		{
			// now we can put this transaction in the processing queue
			logMsg("finished reading the body");
			writeResponse(connection);
			delete this;  // there is no other owner, so we delete ourselves
		}
	}
	else
	{
		logMsg("trouble reading input: %s", error.message().c_str());
		connection->set_status(HTTPServer::connection::internal_server_error);
	}
	logMsg("end asynchronous read callback");
}

void AsyncDaemon::Transaction::writeResponse(HTTPServer::connection_ptr connection)
{
	connection->set_status(HTTPServer::connection::ok);

	array<HTTPServer::response_header, 1> headers =
	{
		{ "Content-Type", "application/json" }
	};

	connection->set_headers(headers);

	connection->write("{ \"err\": 0 }\n");

	logMsg("wrote response");
}

AsyncDaemon::AsyncDaemon(const char* addr, const char* port)
: m_keep_running(true), m_server(0), m_transaction_count(0)
{
	logMsg("AsyncDaemon constructor");

	HTTPServer::options opts(*this);

	opts.address(addr).port(port).reuse_address(true);
	opts.thread_pool(boost::make_shared<boost::network::utils::thread_pool>(2));

	m_server = new HTTPServer(opts);
}

AsyncDaemon::~AsyncDaemon()
{
	logMsg("AsyncDaemon destructor");
	delete m_server;
}

void AsyncDaemon::operator() (
	HTTPServer::request const& request,
	HTTPServer::connection_ptr connection)
{
	int id = m_transaction_count++;

	logMsg("begin transaction %d on thread %lu", id, syscall(SYS_gettid));

	// Wrap a new Transaction object in a unique_ptr so that it will
	// automatically get deleted in case of problems. But in the normal case,
	// however, we don't want it automatically deleted. See below.
	unique_ptr<Transaction> trans { new Transaction(request, id) };

	if (trans->contentLength() == 0)
	{
		// empty queries are errors
		connection->set_status(HTTPServer::connection::bad_request);
	}
	else if (m_keep_running)
	{
		// We must explicitly release the underlying Transaction pointer
		// because it will be owned by the asynchronous callback chain (as a
		// parameter to connection->read(...)) until the complete message body
		// has been read in.
		Transaction* tp = trans.release();

		// begin asynchronously pulling in the query request message body
		tp->asyncRead(connection);
	}
	else
	{
		// we're not handling transactions right now
		connection->set_status(HTTPServer::connection::service_unavailable);
	}

	logMsg("end transaction handler");
}

void AsyncDaemon::initiateShutdown()
{
	logMsg("initiating shutdown");
	m_keep_running = false;

	if (m_server)
	{
		logMsg("stopping server");
		m_server->stop();
	}
}

void AsyncDaemon::log(const char* s) const
{
	logMsg("cpp-netlib message '%s'", s);
}

void AsyncDaemon::receiveLoop()
{
	logMsg("receive thread beginning");

	try
	{
		if (m_server != 0)
		{
			m_server->run();

			logMsg("server run finished");
		}
	}
	catch (exception& e)
	{
		logMsg("HTTP server exception %s", e.what());
		initiateShutdown();
	}

	logMsg("receive thread ending");
}

