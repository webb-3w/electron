#include "atom/browser/electron_single_send_message.h"

#include "base/json/json_reader.h"
#include "base/values.h"

SingletonSendMessage *SingletonSendMessage::GetInstance() {
    static SingletonSendMessage m_Instance;
    return &m_Instance;
}

void SingletonSendMessage::InitKey(std::string md5) {
    mMd5 = md5;
}

std::string SingletonSendMessage::getKey() {
    return mMd5;
}

void SingletonSendMessage::Init(std::string message, std::string filepath) {
  scoped_ptr<base::Value> parsed_message(base::JSONReader::Read(message));
  if (parsed_message && !parsed_message->IsType(base::Value::TYPE_DICTIONARY))
    return;
  url_need_replace.clear();
  url_replace_to.clear();
  base::DictionaryValue* dict = static_cast<base::DictionaryValue*>(parsed_message.get());
  base::DictionaryValue::Iterator iter(*dict);
  while (!iter.IsAtEnd()) {
    url_need_replace.push_back(iter.key());
    std::string value;
    iter.value().GetAsString(&value);
    url_replace_to.push_back(filepath + value);
    iter.Advance();
  }
}

bool SingletonSendMessage::getMatcher(std::string src, std::string *dest) {
  for(size_t i = 0; i < url_need_replace.size(); i++){
    if(src == url_need_replace[i]){
      *dest = url_replace_to[i];
      return true;
    }
  }
  return false;
}
