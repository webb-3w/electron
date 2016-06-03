// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>
#include <utility>
#include <vector>

#include "atom/browser/api/atom_api_window.h"
#include "atom/browser/electron_single_send_message.h"
#include "atom/browser/file_encrypt.h"
#include "atom/browser/native_window.h"
#include "atom/browser/ui/file_dialog.h"
#include "atom/browser/ui/message_box.h"
#include "atom/common/native_mate_converters/callback.h"
#include "atom/common/native_mate_converters/file_path_converter.h"
#include "atom/common/native_mate_converters/image_converter.h"
#include "native_mate/dictionary.h"

#include "base/json/json_reader.h"
#include "base/values.h"
#include "base/md5.h"

#include "atom/common/node_includes.h"

namespace mate {

template<>
struct Converter<file_dialog::Filter> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     file_dialog::Filter* out) {
    mate::Dictionary dict;
    if (!ConvertFromV8(isolate, val, &dict))
      return false;
    if (!dict.Get("name", &(out->first)))
      return false;
    if (!dict.Get("extensions", &(out->second)))
      return false;
    return true;
  }
};

}  // namespace mate

namespace {

void ShowMessageBox(int type,
                    const std::vector<std::string>& buttons,
                    int default_id,
                    int cancel_id,
                    int options,
                    const std::string& title,
                    const std::string& message,
                    const std::string& detail,
                    const gfx::ImageSkia& icon,
                    atom::NativeWindow* window,
                    mate::Arguments* args) {
  v8::Local<v8::Value> peek = args->PeekNext();
  atom::MessageBoxCallback callback;
  if (mate::Converter<atom::MessageBoxCallback>::FromV8(args->isolate(),
                                                        peek,
                                                        &callback)) {
    atom::ShowMessageBox(window, (atom::MessageBoxType)type, buttons,
                         default_id, cancel_id, options, title,
                         message, detail, icon, callback);
  } else {
    int chosen = atom::ShowMessageBox(window, (atom::MessageBoxType)type,
                                      buttons, default_id, cancel_id,
                                      options, title, message, detail, icon);
    args->Return(chosen);
  }
}

void ShowOpenDialog(const std::string& title,
                    const base::FilePath& default_path,
                    const file_dialog::Filters& filters,
                    int properties,
                    atom::NativeWindow* window,
                    mate::Arguments* args) {
  v8::Local<v8::Value> peek = args->PeekNext();
  file_dialog::OpenDialogCallback callback;
  if (mate::Converter<file_dialog::OpenDialogCallback>::FromV8(args->isolate(),
                                                               peek,
                                                               &callback)) {
    file_dialog::ShowOpenDialog(window, title, default_path, filters,
                                properties, callback);
  } else {
    std::vector<base::FilePath> paths;
    if (file_dialog::ShowOpenDialog(window, title, default_path, filters,
                                    properties, &paths))
      args->Return(paths);
  }
}

void ShowSaveDialog(const std::string& title,
                    const base::FilePath& default_path,
                    const file_dialog::Filters& filters,
                    atom::NativeWindow* window,
                    mate::Arguments* args) {
  v8::Local<v8::Value> peek = args->PeekNext();
  file_dialog::SaveDialogCallback callback;
  if (mate::Converter<file_dialog::SaveDialogCallback>::FromV8(args->isolate(),
                                                               peek,
                                                               &callback)) {
    file_dialog::ShowSaveDialog(window, title, default_path, filters, callback);
  } else {
    base::FilePath path;
    if (file_dialog::ShowSaveDialog(window, title, default_path, filters,
                                    &path))
      args->Return(path);
  }
}

#if defined(OS_WIN)
BOOL StringToWString(const std::string &str,std::wstring &wstr)
{
    int nLen = (int)str.length();
    wstr.resize(nLen,L' ');

    int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);

    if (nResult == 0)
    {
        return FALSE;
    }

    return TRUE;
}
#endif

void SetCompanyInfo(const std::string message, const std::string key,
                    mate::Arguments* args) {
  v8::Local<v8::Value> peek = args->PeekNext();
  file_dialog::SaveDialogCallback callback;
  if (mate::Converter<file_dialog::SaveDialogCallback>::FromV8(args->isolate(),
                                                        peek,
                                                        &callback)) {
    FileEncrypt encrypt;
    int ret = encrypt.LoadPublicKey(key);
    if (ret) {
      #if defined(OS_WIN)
      std::wstring value;
      StringToWString("failed to load key", value);
      #else
      std::string value = "failed to load key";
      #endif
      callback.Run(false, base::FilePath(value));
      return;
    }

    std::string decry = encrypt.DecryptString(message);
    scoped_ptr<base::Value> parsed_message(base::JSONReader::Read(decry));
    if (parsed_message && !parsed_message->IsType(base::Value::TYPE_DICTIONARY)) {
      #if defined(OS_WIN)
      std::wstring value;
      StringToWString("failed to load key", value);
      #else
      std::string value = "failed to load key";
      #endif
      callback.Run(false, base::FilePath(value));
      return;
    }
    base::DictionaryValue* dict = static_cast<base::DictionaryValue*>(parsed_message.get());
    if (dict == nullptr) {
      #if defined(OS_WIN)
      std::wstring value;
      StringToWString("failed to decry", value);
      #else
      std::string value = "failed to decry";
      #endif
      callback.Run(false, base::FilePath(value));
      return;
    }
    base::DictionaryValue::Iterator iter(*dict);
    std::string _id;
    std::string website;
    std::string managerServer;
    std::string created;
    std::string url;
    bool privateDeployment = false;
    while (!iter.IsAtEnd()) {
        if (iter.key().compare("_id") == 0) {
          iter.value().GetAsString(&_id);
        }
        if (iter.key().compare("url") == 0) {
          iter.value().GetAsString(&url);
        }
        if (iter.key().compare("website") == 0) {
          iter.value().GetAsString(&website);
        }
        if (iter.key().compare("managerServer") == 0) {
          iter.value().GetAsString(&managerServer);
        }
        if (iter.key().compare("created") == 0) {
          iter.value().GetAsString(&created);
        }
        if (iter.key().compare("privateDeployment") == 0) {
          iter.value().GetAsBoolean(&privateDeployment);
        }
        iter.Advance();
    }
    std::string hash;
    {
        std::string tpl = "{\"_id\":\"" + _id +
                        "\",\"website\":\"" + website +
                        "\",\"managerServer\":\"" + managerServer +
                        "\",\"created\":\"" + created +
                        "\",\"privateDeployment\":" + (privateDeployment? "true":"false") +
                        "}";
        base::MD5Digest expected_digest;
        base::MD5Sum(tpl.c_str(), tpl.length(), &expected_digest);
        hash += base::MD5DigestToBase16(expected_digest);
    }
    {
        std::string tpl = "{\"website\":\"" + website +
                        "\",\"managerServer\":\"" + managerServer +
                        "\",\"created\":\"" + created +
                        "\",\"privateDeployment\":" + (privateDeployment? "true":"false") +
                        ",\"_id\":\"" + _id +
                        "\"}";
        base::MD5Digest expected_digest;
        base::MD5Sum(tpl.c_str(), tpl.length(), &expected_digest);
        hash += base::MD5DigestToBase16(expected_digest);
    }
    {
        std::string tpl = "{\"managerServer\":\"" + managerServer +
                        "\",\"created\":\"" + created +
                        "\",\"privateDeployment\":" + (privateDeployment? "true":"false") +
                        ",\"_id\":\"" + _id +
                        "\",\"website\":\"" + website +
                        "\"}";
        base::MD5Digest expected_digest;
        base::MD5Sum(tpl.c_str(), tpl.length(), &expected_digest);
        hash += base::MD5DigestToBase16(expected_digest);
    }
    {
        std::string tpl = "{\"created\":\"" + created +
                        "\",\"privateDeployment\":" + (privateDeployment? "true":"false") +
                        ",\"_id\":\"" + _id +
                        "\",\"website\":\"" + website +
                        "\",\"managerServer\":\"" + managerServer +
                        "\"}";
        base::MD5Digest expected_digest;
        base::MD5Sum(tpl.c_str(), tpl.length(), &expected_digest);
        hash += base::MD5DigestToBase16(expected_digest);
    }
    SingletonSendMessage::GetInstance()->InitKey(hash);
    #if defined(OS_WIN)
    std::wstring value;
    StringToWString(url, value);
    #else
    std::string value = url;
    #endif
    callback.Run(true, base::FilePath(value));
  }
}

void SourceOverload(const std::string message, const std::string file_path) {
  SingletonSendMessage::GetInstance()->Init(message, file_path);
}

void Initialize(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context, void* priv) {
  mate::Dictionary dict(context->GetIsolate(), exports);
  dict.SetMethod("setCompanyInfo", &SetCompanyInfo);
  dict.SetMethod("sourceOverload", &SourceOverload);
  dict.SetMethod("showMessageBox", &ShowMessageBox);
  dict.SetMethod("showErrorBox", &atom::ShowErrorBox);
  dict.SetMethod("showOpenDialog", &ShowOpenDialog);
  dict.SetMethod("showSaveDialog", &ShowSaveDialog);
}

}  // namespace

NODE_MODULE_CONTEXT_AWARE_BUILTIN(atom_browser_dialog, Initialize)
