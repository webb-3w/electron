#ifndef ATOM_BROWSER_SINGLETONSENDMESSAGE_H_
#define ATOM_BROWSER_SINGLETONSENDMESSAGE_H_

#include <string>
#include "base/values.h"

class SingletonSendMessage
{
public:
    static SingletonSendMessage *GetInstance();
    void Init(std::string message, std::string filepath);
    void InitKey(std::string md5);
    std::string getKey();
    bool getMatcher(std::string src, std::string *dest);
private:
    SingletonSendMessage()
    {
    }
    std::string mMd5;
    std::vector<std::string> url_need_replace;
    std::vector<std::string> url_replace_to;
};

#endif
