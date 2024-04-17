#pragma once

#include <QTextEdit>
#include <QLabel>
#include <QString>

#include <iostream>
#include <sstream>



class StreamBufBase : public std::streambuf {
protected:
   std::stringstream os;
public:
   StreamBufBase(void) : std::streambuf() { };
   virtual ~StreamBufBase(void) { };

   virtual void Write(void) = 0;

   virtual int_type overflow(int_type c) override {
      switch (c) {
         case '\n':
            Write();
            os.str("");
            break;
         [[likely]] default:
            os.put(c);
         }
      return c;
      }
   };

class MemoStreamBuf : public StreamBufBase {
   private:
      QTextEdit* value;
   public:
      MemoStreamBuf(QTextEdit* para, bool boClean = true) : StreamBufBase() {
         value = para;
         if (boClean) value->clear();
         }

      virtual ~MemoStreamBuf(void) { value = nullptr; }

      virtual void Write(void) override {
         if (this->os.str().length() > 0) [[likely]] {
            value->moveCursor(QTextCursor::End);
            value->insertPlainText(QString::fromStdString(os.str()));
            value->insertPlainText("\n");
            }
         else {
            value->append("");
            }
         }
   };

class LabelStreamBuf : public StreamBufBase {
   private:
      QLabel* value;
   public:
      LabelStreamBuf(QLabel* para, bool boClean = true) : StreamBufBase() {
         value = para;
         if (boClean) value->clear();
         }
      LabelStreamBuf(void) = delete;
      LabelStreamBuf(LabelStreamBuf const&) = delete;

      virtual ~LabelStreamBuf(void) { value = nullptr; }

      virtual void Write(void) override {
         value->setText(QString::fromStdString(os.str()));
         }
   };

class TStreamWrapper {
private:
   std::ostream& str;
   std::streambuf* old = nullptr;
public:
   TStreamWrapper(void) = delete;
   TStreamWrapper(TStreamWrapper const&) = delete;

   TStreamWrapper(std::ostream& ref) : str(ref) { old = nullptr; }
   ~TStreamWrapper(void) {
      if (old != nullptr) {
         delete str.rdbuf(old);
      }
   }

   void Reset(void) {
      if (old != nullptr) {
         delete str.rdbuf(old);
         old = nullptr;
      }
   }

   void Activate(QTextEdit* elem, bool clean = false) {
      Reset();
      old = str.rdbuf(new MemoStreamBuf(elem, clean));
      }

   void Activate(QLabel* elem, bool clean = false) {
      Reset();
      old = str.rdbuf(new LabelStreamBuf(elem, clean));
      }
};

// --------------------------------------------------------------------

class WStreamBufBase : public std::wstreambuf {
protected:
   std::wstringstream os;
public:
   WStreamBufBase(void) : std::wstreambuf() { };
   virtual ~WStreamBufBase(void) { };

   virtual void Write(void) = 0;

   virtual int_type overflow(int_type c) override {
      switch (c) {
      case L'\n':
         Write();
         os.str(L"");
         break;
      [[likely]] default:
         os.put(c);
      }
      return c;
   }
};

class WMemoStreamBuf : public WStreamBufBase {
private:
   QTextEdit* value;
public:
   WMemoStreamBuf(QTextEdit* para, bool boClean = true) : WStreamBufBase() {
      value = para;
      if (boClean) value->clear();
   }

   virtual ~WMemoStreamBuf(void) { value = nullptr; }

   virtual void Write(void) override {
      if (this->os.str().length() > 0) [[likely]] {
         value->moveCursor(QTextCursor::End);
         value->insertPlainText(QString::fromStdWString(os.str()));
         value->insertPlainText(QString("\n"));
         }
      else {
         value->append(QString(""));
      }
   }
};


class TWStreamWrapper {
private:
   std::wostream& str;
   std::wstreambuf* old = nullptr;
public:
   TWStreamWrapper(void) = delete;
   TWStreamWrapper(TWStreamWrapper const&) = delete;
   
   TWStreamWrapper(TWStreamWrapper&&) = default;
   TWStreamWrapper(std::wostream& ref) : str(ref) { old = nullptr; }
   ~TWStreamWrapper(void) {
      if (old != nullptr) {
         delete str.rdbuf(old);
      }
   }

   void Reset(void) {
      if (old != nullptr) {
         delete str.rdbuf(old);
         old = nullptr;
      }
   }

   void Activate(QTextEdit* elem, bool clean = false) {
      Reset();
      old = str.rdbuf(new WMemoStreamBuf(elem, clean));
   }

};
