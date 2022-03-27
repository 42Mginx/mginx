#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include "ConfigUtil.hpp"

#define directivesMap std::map<std::string, void (ServerBlock::*)(std::vector<std::string>)>
class ServerBlock
{
private:
	static  ServerBlock				_default_conf;

	std::vector<std::string> _server_name;	// 여러개일 수 있음
	std::vector<t_listen> _listen;			// 여러개일 수 있음
	std::string _root;						// 얘는 한개
	std::set<std::string> _allowed_methods; // 얘는 여러개
	std::map<int, std::string> _error_page; // 얘도 여러개
	int _client_body_buffer_size;			// 한 개
	bool _auto_index;						// 한 개

	// location directives
	std::map<std::string, std::string> _cgi_param;
	std::string _cgi_pass;
	std::vector<std::string> _index;
	std::string _alias;
	bool _aliasSet;

	std::map<std::string, ServerBlock> _location; // path, 해당 path location 클래스

public:
	ServerBlock();
	~ServerBlock();
	ServerBlock &operator=(ServerBlock const &src);

	std::vector<std::string> getServerName() const;
	std::vector<t_listen> getListen() const;
	std::string getRoot() const;
	std::set<std::string> getAllowedMethods() const;
	std::map<int, std::string> getErrorPage() const;
	int getClientBodyBufferSize() const;
	bool getAutoIndex() const;
	std::vector<std::string> getIndex() const;

	std::map<std::string, ServerBlock> getLocation() const;
	std::map<std::string, std::string> getCgiParam() const;
	std::string getCgiPass() const;
	std::string getAlias() const;
	bool getAliasSet() const;

	// 지시어 파싱 함수
	void addServerName(std::vector<std::string> args);
	void addListen(std::vector<std::string> args);
	void addRoot(std::vector<std::string> args);
	void addAllowedMethods(std::vector<std::string> args);
	void addErrorPage(std::vector<std::string> args);
	void addClientBodyBufferSize(std::vector<std::string> args);
	void addAutoIndex(std::vector<std::string> args);

	void addCgiParam(std::vector<std::string> args);
	void addCgiPass(std::vector<std::string> args);
	void addIndex(std::vector<std::string> args);
	void addAlias(std::vector<std::string> args);

	static void _initDefaultServer(const char *filename);

	int parseServerBlock(unsigned int &index, fileVector &file);
	int parseLocationBlock(unsigned int &index, fileVector &file);
	static directivesMap directivesParseFunc;
	static directivesMap locationDirectivesParseFunc;
	// 지시어(directives) 모음 맵
	static directivesMap _initDirectivesMap();
	static directivesMap _initLocationDirectivesMap();
	void	passMembers(ServerBlock &server) const;
	class ExceptionInvalidArguments : public std::exception
	{
		virtual const char *what() const throw();
	};
};

#endif
