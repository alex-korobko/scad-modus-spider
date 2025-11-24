#ifndef __SOCKET_EXCEPTION_H__
#define __SOCKET_EXCEPTION_H__

//defenition of socket exception class

class socket_exception
{
	public:
		explicit socket_exception (string s) : m_s (s) {};
		virtual ~socket_exception () {};
		
		string get_description () {return m_s;};
	private:
		string m_s;
};
#endif
