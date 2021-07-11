#include"log.h"
#include<functional>
#include<tuple>
#include"config.h"

namespace sylar {
	
	const char* LogLevel::ToString(LogLevel::Level level) {
		switch (level){
		#define xxx(name) \
			case LogLevel::name:\
				return #name;\
				break;
			xxx(DEBUG);
			xxx(INFO);
			xxx(WARN);
			xxx(ERROR);
			xxx(FATAL);
		#undef xxx(name)
		default:
			return "UNKNOW";
		}
		return "UNKNOW";
	}
	LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(name) \
		if (str == #name) { \
			return LogLevel::name; \
		}
		XX(DEBUG);
		XX(INFO);
		XX(WARN);
		XX(ERROR);
		XX(FATAL);
		return LogLevel::UNKNOW;
#undef XX
	}

	LogEvent::LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level
		,const char* file, int32_t line,uint32_t elapse
		, uint32_t thread_id,uint32_t fiber_id, uint64_t time)
	:m_file(file)
	,m_line(line)
	, m_elapse(elapse)
	, m_threadId(thread_id)
	, m_fiberId(fiber_id)
	, m_time(time)
	, m_logger(logger)
	, m_level(level)
	, m_ss("xxx")
	, m_threadName("root"){

	}
	void LogEvent::format(const char* fmt, ...) {
		va_list al;
		va_start(al, fmt);
		format(fmt, al);
		va_end(al);
	}

	void LogEvent::format(const char* fmt, va_list al) {
		char buf[1024];
		int len = vsprintf_s(buf, fmt, al);
		//int len = vsprintf_s(buf, fmt, al);
		if (len != -1) {
			std::string str(buf, buf + len);
			m_ss << str;
		}
	}

	LogEventWrap::LogEventWrap(LogEvent::ptr e)
		:m_event(e) {
	}
	LogEventWrap::~LogEventWrap() {
		m_event->getLogger()->log(m_event->getLevel(), m_event);
	}

	std::stringstream& LogEventWrap::getSS() {
		return m_event->getSS();
	}

	std::string LogFormatter::format(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
		std::stringstream ss;
		
		//for (auto& i : m_items) {
		for(auto& i:logger->getFormatter()->m_items){
			i->format(ss, logger, level, event);
		}
		return ss.str();
	}
	LogFormatter::LogFormatter(const std::string& pattern): m_pattern(pattern) {
		init();
	}

	class MessageFormatItem :public LogFormatter::FormatItem {
	public:
		MessageFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getContent();
		}
	};
	class LevelFormatItem :public LogFormatter::FormatItem {
	public:
		LevelFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << LogLevel::ToString(level);
		}
	};
	class ElapseFormatItem : public LogFormatter::FormatItem {
	public:
		ElapseFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getElapse();
		}
	};

	class NameFormatItem : public LogFormatter::FormatItem {
	public:
		NameFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getLogger()->getName();
		}
	};

	class ThreadIdFormatItem : public LogFormatter::FormatItem {
	public:
		ThreadIdFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getThreadId();
		}
	};

	class FiberIdFormatItem : public LogFormatter::FormatItem {
	public:
		FiberIdFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getFiberId();
		}
	};

	class ThreadNameFormatItem : public LogFormatter::FormatItem {
	public:
		ThreadNameFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getThreadName();
		}
	};
	class DateTimeFormatItem : public LogFormatter::FormatItem {
	public:
		DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
			:m_format(format) {
			if (m_format.empty()) {
				m_format = "%Y-%m-%d %H:%M:%S";
			}
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			struct tm tm;
			time_t time = event->getTime();
			localtime_s(&tm, &time);
			char buf[64];
			strftime(buf, sizeof(buf), m_format.c_str(), &tm);
			os << buf;
		}
	private:
		std::string m_format;
	};
	class FilenameFormatItem : public LogFormatter::FormatItem {
	public:
		FilenameFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getFile();
		}
	};

	class LineFormatItem : public LogFormatter::FormatItem {
	public:
		LineFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getLine();
		}
	};

	class NewLineFormatItem : public LogFormatter::FormatItem {
	public:
		NewLineFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << std::endl;
		}
	};


	class StringFormatItem : public LogFormatter::FormatItem {
	public:
		StringFormatItem(const std::string& str)
			:m_string(str) {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << m_string;
		}
	private:
		std::string m_string;
	};

	class TabFormatItem : public LogFormatter::FormatItem {
	public:
		TabFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << "\t";
		}
	private:
		std::string m_string;
	};

    //%xxx %xxx{xxx} %%
	void LogFormatter::init() {
		//str,format,type
		std::vector<std::tuple<std::string, std::string, int>> vec;
		std::string nstr;
		for (size_t i = 0; i < m_pattern.size(); ++i) {
			if (m_pattern[i] != '%') {
				nstr.append(1, m_pattern[i]);
				continue;
			}

			if ((i + 1) < m_pattern.size()) {
				if (m_pattern[i + 1] == '%') {
					nstr.append(1, '%');
					continue;
				}
			}

			size_t n = i + 1;
			int fmt_status = 0;
			size_t fmt_begin = 0;

			std::string str;
			std::string fmt;
			while (n < m_pattern.size()) {
				if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
					&& m_pattern[n] != '}')) {
					str = m_pattern.substr(i + 1, n - i - 1);
					break;
				}
				if (fmt_status == 0) {
					if (m_pattern[n] == '{') {
						str = m_pattern.substr(i + 1, n - i - 1);
						//std::cout << "*" << str << std::endl;
						fmt_status = 1; //������ʽ
						fmt_begin = n;
						++n;
						continue;
					}
				}
				else if (fmt_status == 1) {
					if (m_pattern[n] == '}') {
						fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
						//std::cout << "#" << fmt << std::endl;
						fmt_status = 0;
						++n;
						break;
					}
				}
				++n;
				if (n == m_pattern.size()) {
					if (str.empty()) {
						str = m_pattern.substr(i + 1);
					}
				}
			}

			if (fmt_status == 0) {
				if (!nstr.empty()) {
					vec.push_back(std::make_tuple(nstr, std::string(), 0));
					nstr.clear();
				}
				vec.push_back(std::make_tuple(str, fmt, 1));
				i = n - 1;
			}
			else if (fmt_status == 1) {
				std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
				m_error = true;
				vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
			}
		}

		if (!nstr.empty()) {
			vec.push_back(std::make_tuple(nstr, "", 0));
		}
		static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
	#define XX(str, C) \
	        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

			XX(m, MessageFormatItem),           //m:��Ϣ
			XX(p, LevelFormatItem),             //p:����ȼ�
			XX(r, ElapseFormatItem),            //r:�ۼƺ�����
			XX(c, NameFormatItem),              //c:��־����
			XX(t, ThreadIdFormatItem),          //t:�߳�id
			XX(n, NewLineFormatItem),           //n:����
			XX(d, DateTimeFormatItem),          //d:ʱ��
			XX(f, FilenameFormatItem),          //f:�ļ���
			XX(l, LineFormatItem),              //l:�к�
			XX(T, TabFormatItem),               //T:Tab
			XX(F, FiberIdFormatItem),           //F:Э��id
			XX(N, ThreadNameFormatItem),        //N:�߳�����
	#undef XX
		};

		for (auto& i : vec) {
			if (std::get<2>(i) == 0) {
				m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
			}
			else {
				auto it = s_format_items.find(std::get<0>(i));
				if (it == s_format_items.end()) {
					m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
					m_error = true;
				}
				else {
					m_items.push_back(it->second(std::get<1>(i)));
				}
			}

			//std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
		}
		//std::cout << m_items.size() << std::endl;
	}

	Logger::Logger(const std::string name)
	:m_name(name),m_level(LogLevel::DEBUG) {
		m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
		if (name == "root") {
			m_appenders.push_back(LogAppender::ptr(new StdoutLogAppender()));
		}
	}
	void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
		if (level >= m_level) {
			auto self = shared_from_this();
			if (!m_appenders.empty()) {
				for (auto& it : m_appenders) {
					it->log(self,level, event);
				}
			}
			else if (m_root) {
				m_root->log(level, event);
			}
			
		}
	}
	void Logger::addAppender(LogAppender::ptr appender) {
		if (!appender->getFormatter()) {
			appender->m_formatter= m_formatter;   
			//�����ӵ�appender��formatterΪ�գ���ô��m_formatter������
		}
		m_appenders.push_back(appender);
	}
	void Logger::delAppender(LogAppender::ptr appender) {
		for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
			if (*it == appender) {
				m_appenders.erase(it);
				break;
			}
		}
	}
	void Logger::clearAppenders() {
		m_appenders.clear();
	}
	void Logger::setFormatter(LogFormatter::ptr val) {
		m_formatter = val;
	}
	void Logger::setFormatter(const std::string& val) {
		sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(val));
		if (new_val->isError()) {
			std::cout << "Logger setFormatter name=" << m_name
				<< " value=" << val << " invalid formatter"
				<< std::endl;
			return;
		}
		m_formatter=new_val;
	}
	LogFormatter::ptr Logger::getFormatter() {
		return m_formatter;
	}
	void Logger::debug(LogEvent::ptr event) {
		log(LogLevel::DEBUG, event);
	}
	void Logger::info(LogEvent::ptr event) {
		log(LogLevel::INFO, event);
	}
	void Logger::warn(LogEvent::ptr event) {
		log(LogLevel::WARN, event);
	}
	void Logger::error(LogEvent::ptr event) {
		log(LogLevel::ERROR, event);
	}
	void Logger::fatal(LogEvent::ptr event) {
		log(LogLevel::FATAL, event);
	}

	LoggerManager::LoggerManager() {
		m_root.reset(new Logger);
		m_root->addAppender(LogAppender::ptr(new StdoutLogAppender()));
		m_loggers[m_root->m_name] = m_root;

		init();
	}

	Logger::ptr LoggerManager::getLogger(const std::string name) {
		auto it = m_loggers.find(name);
		if (it != m_loggers.end()) {
			return it->second;
		}

		Logger::ptr logger(new Logger(name));
		logger->m_root = m_root;
		logger->addAppender(LogAppender::ptr(new StdoutLogAppender()));
		m_loggers[name] = logger;
		return logger;
	}


	struct LogAppenderDefine{
		int type = 0;  //1:File 2:Stdout
		LogLevel::Level level = LogLevel::UNKNOW;
		std::string formatter;
		std::string file;

		bool operator==(const LogAppenderDefine& oth)const {
			return type == oth.type
				&& level == oth.level
				&& formatter == oth.formatter
				&& file == oth.file;
		}
	};
	struct LogDefine{
		std::string name;
		LogLevel::Level level = LogLevel::UNKNOW;
		std::string formatter;
		std::vector<LogAppenderDefine> appenders;

		bool operator==(const LogDefine& oth)const {
			return name == oth.name
				&& level == oth.level
				&& formatter == oth.formatter
				&& appenders == oth.appenders;
		}
		bool operator<(const LogDefine& oth)const {
			return name < oth.name;
		}
	};

	template<>
	class LexicalCast<std::string, LogDefine> {
	public:
		LogDefine operator()(const std::string& v) {
			YAML::Node n = YAML::Load(v);
			LogDefine ld;
			if (!n["name"].IsDefined()) {
				std::cout << "log config error: name is null, " << n
					<< std::endl;
				throw std::logic_error("log config name is null");
			}
			ld.name = n["name"].as<std::string>();
			ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
			if (n["formatter"].IsDefined()) {
				ld.formatter = n["formatter"].as<std::string>();
			}

			if (n["appenders"].IsDefined()) {
				//std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
				for (size_t x = 0; x < n["appenders"].size(); ++x) {
					auto a = n["appenders"][x];
					if (!a["type"].IsDefined()) {
						std::cout << "log config error: appender type is null, " << a
							<< std::endl;
						continue;
					}
					std::string type = a["type"].as<std::string>();
					LogAppenderDefine lad;
					if (type == "FileLogAppender") {
						lad.type = 1;
						if (!a["file"].IsDefined()) {
							std::cout << "log config error: fileappender file is null, " << a
								<< std::endl;
							continue;
						}
						lad.file = a["file"].as<std::string>();
						if (a["formatter"].IsDefined()) {
							lad.formatter = a["formatter"].as<std::string>();
						}
					}
					else if (type == "StdoutLogAppender") {
						lad.type = 2;
						if (a["formatter"].IsDefined()) {
							lad.formatter = a["formatter"].as<std::string>();
						}
					}
					else {
						std::cout << "log config error: appender type is invalid, " << a
							<< std::endl;
						continue;
					}

					ld.appenders.push_back(lad);
				}
			}
			return ld;
		}
	};

	template<>
	class LexicalCast<LogDefine, std::string> {
	public:
		std::string operator()(const LogDefine& i) {
			YAML::Node n;
			n["name"] = i.name;
			if (i.level != LogLevel::UNKNOW) {
				n["level"] = LogLevel::ToString(i.level);
			}
			if (!i.formatter.empty()) {
				n["formatter"] = i.formatter;
			}

			for (auto& a : i.appenders) {
				YAML::Node na;
				if (a.type == 1) {
					na["type"] = "FileLogAppender";
					na["file"] = a.file;
				}
				else if (a.type == 2) {
					na["type"] = "StdoutLogAppender";
				}
				if (a.level != LogLevel::UNKNOW) {
					na["level"] = LogLevel::ToString(a.level);
				}

				if (!a.formatter.empty()) {
					na["formatter"] = a.formatter;
				}

				n["appenders"].push_back(na);
			}
			std::stringstream ss;
			ss << n;
			return ss.str();
		}
	};

	sylar::ConfigVar<std::set<LogDefine> >::ptr g_log_defines = 
		sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

	struct LogIniter {
		LogIniter() {
			g_log_defines->addListener([](const std::set<LogDefine>& old_value,
				const std::set<LogDefine>& new_value) {
					SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed";
					for (auto& i : new_value) {
						auto it = old_value.find(i);
						sylar::Logger::ptr logger;
						if (it == old_value.end()) {
							//����logger
							logger = SYLAR_LOG_NAME(i.name);
						}
						else {
							if (!(i == *it)) {
								//�޸ĵ�logger
								logger = SYLAR_LOG_NAME(i.name);
							}
							else {
								continue;
							}
						}
						logger->setLevel(i.level);
						//std::cout << "** " << i.name << " level=" << i.level
						//<< "  " << logger << std::endl;
						if (!i.formatter.empty()) {
							logger->setFormatter(i.formatter);
						}

						logger->clearAppenders();
						for (auto& a : i.appenders) {
							sylar::LogAppender::ptr ap;
							if (a.type == 1) {
								ap.reset(new FileLogAppender(a.file));
							}
							else if (a.type == 2) {
								//if (!sylar::EnvMgr::GetInstance()->has("d")) {
									ap.reset(new StdoutLogAppender);
								//}
								//else {
									continue;
								//}
							}
							ap->setLevel(a.level);
							if (!a.formatter.empty()) {
								LogFormatter::ptr fmt(new LogFormatter(a.formatter));
								if (!fmt->isError()) {
									ap->setFormatter(fmt);
								}
								else {
									std::cout << "log.name=" << i.name << " appender type=" << a.type
										<< " formatter=" << a.formatter << " is invalid" << std::endl;
								}
							}
							logger->addAppender(ap);
						}
					}

					for (auto& i : old_value) {
						auto it = new_value.find(i);
						if (it == new_value.end()) {
							//ɾ��logger
							auto logger = SYLAR_LOG_NAME(i.name);
							logger->setLevel((LogLevel::Level)0);
							logger->clearAppenders();
						}
					}
				});
		}
	};
	static LogIniter __log_init;

	void LoggerManager::init() {
		;
	}

	FileLogAppender::FileLogAppender(const std::string& filename): m_filename(filename) {

	}
	void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
		if (level >= m_level) {
			m_filestream << m_formatter->format(logger,level,event);
		}
	}
	bool FileLogAppender::reopen() {
		if (m_filestream) {
			m_filestream.close();
		}
		m_filestream.open(m_filename);
		return !!m_filestream;
	}
	void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
		if (level >= m_level) {
			std::cout << m_formatter->format(logger, level, event);
		}
	}
}