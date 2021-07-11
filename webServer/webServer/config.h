#pragma once
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <memory>
#include<string>
#include <sstream>
#include"log.h"
#include"yaml-cpp/yaml.h"
#include <boost/lexical_cast.hpp>
#include<vector>
#include<list>
#include<map>
#include<set>
#include<unordered_map>
#include<unordered_set>
#include<functional>

namespace sylar {
	class ConfigVarBase {
	public:
		typedef std::shared_ptr<ConfigVarBase> ptr;
		ConfigVarBase(const std::string& name, const std::string& description = "")
			:m_name(name)
			, m_description(description) {
			std::transform(m_name.begin(), m_name.end(),m_name.begin(), ::tolower);
		}
		virtual ~ConfigVarBase() {}

		const std::string& getName() const { return m_name; }
		const std::string& getDescription() const { return m_description; }

		virtual std::string toString() = 0;
		virtual bool fromString(const std::string& val) = 0;
		virtual std::string getTypeName() const = 0;
	protected:
		std::string m_name;
		std::string m_description;
	};

	template<class F, class T>
	class LexicalCast {
	public:
		T operator()(const F& v) {
			return boost::lexical_cast<T>(v);
			/*******************ԭ���Լ�д��************************/
			////std::cout << typeid(v).name() << std::endl;
			//if (typeid(v).name() == typeid(std::string).name()) {
			//	T res;
			//	std::istringstream os(v);
			//	os >> res;
			//	os.clear();
			//	//std::cout << typeid(res).name() << std::endl;
			//	return res;
			//}
			//T res;
			//std::ostringstream os;
			//os << v;
			//std::istringstream i(os.str());
			//i >> res;
			//return res;
			///*************�㲻��Ϊʲô����ֱ�����£�����������һ����ȴ��ʾ�޷�ת��************************/
			////	std::ostringstream os;
			////	os << v;
			////	return os.str();
			/***********************************************************/
		}
	};

	template<class T>
	class LexicalCast<std::string, std::vector<T> > {
	public:
		std::vector<T> operator() (const std::string& v) {
			YAML::Node node = YAML::Load(v);
			typename std::vector<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++) {
				ss.str("");
				ss << node[i];
				vec.push_back(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};
	template<class T>
	class LexicalCast<std::vector<T>, std::string > {
	public:
		std::string operator() (const std::vector<T>& v) {
			YAML::Node node;
			for (auto& i : v) {
				node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::list<T> > {
	public:
		std::list<T> operator() (const std::string& v) {
			YAML::Node node = YAML::Load(v);
			typename std::list<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++) {
				ss.str("");
				ss << node[i];
				vec.push_back(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};
	template<class T>
	class LexicalCast<std::list<T>, std::string > {
	public:
		std::string operator() (const std::list<T>& v) {
			YAML::Node node;
			for (auto& i : v) {
				node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::set<T> > {
	public:
		std::set<T> operator() (const std::string& v) {
			YAML::Node node = YAML::Load(v);
			typename std::set<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++) {
				ss.str("");
				ss << node[i];
				vec.insert(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};
	template<class T>
	class LexicalCast<std::set<T>, std::string > {
	public:
		std::string operator() (const std::set<T>& v) {
			YAML::Node node;
			for (auto& i : v) {
				node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::unordered_set<T> > {
	public:
		std::unordered_set<T> operator() (const std::string& v) {
			YAML::Node node = YAML::Load(v);
			typename std::unordered_set<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++) {
				ss.str("");
				ss << node[i];
				vec.insert(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};
	template<class T>
	class LexicalCast<std::unordered_set<T>, std::string > {
	public:
		std::string operator() (const std::unordered_set<T>& v) {
			YAML::Node node;
			for (auto& i : v) {
				node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::map<std::string,T> > {
	public:
		std::map<std::string, T> operator() (const std::string& v) {
			YAML::Node node = YAML::Load(v);
			typename std::map<std::string, T> vec;
			std::stringstream ss;
			for (auto it = node.begin(); it != node.end(); it++) {
				ss.str("");
				ss << it->second;
				vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
			}
			return vec;
		}
	};
	template<class T>
	class LexicalCast<std::map<std::string, T>, std::string > {
	public:
		std::string operator() (const std::map<std::string, T>& v) {
			YAML::Node node;
			for (auto& i : v) {
				node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::unordered_map<std::string, T> > {
	public:
		std::unordered_map<std::string, T> operator() (const std::string& v) {
			YAML::Node node = YAML::Load(v);
			typename std::unordered_map<std::string, T> vec;
			std::stringstream ss;
			for (auto it = node.begin(); it != node.end(); it++) {
				ss.str("");
				ss << it->second;
				vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
			}
			return vec;
		}
	};
	template<class T>
	class LexicalCast<std::unordered_map<std::string, T>, std::string > {
	public:
		std::string operator() (const std::unordered_map<std::string, T>& v) {
			YAML::Node node;
			for (auto& i : v) {
				node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	//FromStr  T operator()(const std::string& str)
	//ToStr std::string operator()(const T& val)
	template<class T, class FromStr=LexicalCast<std::string,T>, class ToStr=LexicalCast<T,std::string>>
	class ConfigVar : public ConfigVarBase {
	public:
		typedef std::shared_ptr<ConfigVar> ptr;
		typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

		ConfigVar(const std::string& name, const T& default_value, const std::string& description = "") 
			:ConfigVarBase(name,description)
		,m_val(default_value){
		}
		std::string toString() override {
			try {
				/******************************/
				//std::ostringstream os;
				//os << m_val;
				//return os.str();
				/******************************/
				return ToStr()(m_val);
			}
			catch(std::exception& e){
				SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
					<< e.what() << "convert: " << typeid(m_val).name() << "to string";
			}
			return "";
		}
		bool fromString(const std::string& val) override {
			try {
				/************************************/
				//std::istringstream os(val);
				//os >> m_val;
				/***********************************/
				setValue(FromStr()(val));
				return true;
			}
			catch (std::exception& e) {
				SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString exception"
					<< e.what() << " convert: string to " << typeid(m_val).name();
			}
			return false;
		}

		const T getValue() const { return m_val; }
		void setValue(const T& v) { 
			if (v == m_val) {
				return;
			}
			for (auto& i : m_cbs) {
				i.second(m_val,v);
			}
			m_val = v;
		}
		std::string getTypeName() const override{ return typeid(T).name(); }

		void addListener(uint64_t key, on_change_cb cb){
			m_cbs[key] = cb;
		}
		uint64_t addListener(on_change_cb cb) {
			static uint64_t s_fun_id = 0;
			++s_fun_id;
			m_cbs[s_fun_id] = cb;
			return s_fun_id;
		}
		void delListener(uint16_t key) {
			m_cbs.erase(key);
		}
		void clearListener() {
			m_cbs.clear();
		}
		on_change_cb getListener(uint16_t key) const {
			auto it = m_cbs.find(key);
			return it == m_cbs.end() ? nullptr : it->second;
		}

	private:
		T m_val;
		std::map<uint64_t, on_change_cb> m_cbs;
	};
	class Config {
	public:
		typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
		template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value
			, const std::string& description = "") {
			//auto tmp = Lookup<T>(name);
			auto it = GetDatas().find(name);
			if (it != GetDatas().end()) {
				auto tmp = std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
				if (tmp) {
					SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists";
					return tmp;
				}
				else {
					SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
						<< typeid(T).name() << " real_type=" << it->second->getTypeName()
						<< " " << it->second->toString();
					return nullptr;
				}
			}
			
			if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
				!= std::string::npos) {
				SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid " << name;
				throw std::invalid_argument(name);
			}
			typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
			GetDatas()[name] = v;
			return v;
		}
		template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
			auto it = GetDatas().find(name);
			if (it == GetDatas().end()) {
				return nullptr;
			}
			return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
		}
		static void LoadFromYaml(const YAML::Node& root);
		static ConfigVarBase::ptr LookupBase(const std::string& name);
	private:
		static ConfigVarMap& GetDatas() {  //��̬��Ա��ʼ��˳������
			static ConfigVarMap s_datas;
			return s_datas;
		}
		
	};
}

#endif // !__CONFIG_H__
