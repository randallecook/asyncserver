#include <boost/network/protocol/http/server.hpp>
#include <memory>

class AsyncDaemon;

typedef boost::network::http::async_server<AsyncDaemon> HTTPServer;

class AsyncDaemon
{
	class Transaction
	{
		int m_id;
		size_t m_content_length;
		std::string m_request_body;
		HTTPServer::connection_ptr m_connection;

		public:
		Transaction(HTTPServer::request const& request, int id);
		~Transaction();

		void asyncRead(HTTPServer::connection_ptr connection);
		void writeResponse(HTTPServer::connection_ptr connection);

		size_t contentLength() const { return m_content_length; }
		int id() const { return m_id; }

		void operator()(
			HTTPServer::connection::input_range r,
			boost::system::error_code error,
			size_t n,
			HTTPServer::connection_ptr connection);
	};

	bool m_keep_running;
	HTTPServer* m_server;
	int m_transaction_count;

public:
	AsyncDaemon(const char* addr, const char* port);
	~AsyncDaemon();

	void operator() (
		HTTPServer::request const& request,
		HTTPServer::connection_ptr connection);

	bool isRunningNormally() const { return m_keep_running; }
	int transactions() const { return m_transaction_count; }

	void initiateShutdown();
	void log(const char* s) const;
	void receiveLoop();
};

void logMsg(const char* format, ...);

