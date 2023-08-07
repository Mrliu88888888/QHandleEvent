#include "HandleEvent.h"
#include <qjsondocument.h>
#include <qjsonobject.h>

const QMap<HandleEvent::Result::ECode, QString> HandleEvent::Result::sm_mapCodeDesc = {
	{HandleEvent::Result::ECode::Fail, "失败"},
	{HandleEvent::Result::ECode::Succ, "成功"},
	{HandleEvent::Result::ECode::NotRegEvent, "没有注册此事件的回调"},
	{HandleEvent::Result::ECode::NotFoundSeparator, "没有找到分隔符"}
};

HandleEvent::Result::Result(const Result::ECode& _code, const QString& _content)
	: code(_code)
	, content(_content)
{
}

QMap<QString, HandleEvent*> HandleEvent::sm_mapHandle;
const QChar HandleEvent::sm_commentChar = '=';
QReadWriteLock HandleEvent::sm_rwmutHandle;

HandleEvent::HandleEvent(const QString& eventKey)
	: m_eventHandleKey(eventKey)
{
	this->addHandle();
}

HandleEvent::~HandleEvent()
{
	this->delHandle();
}

void HandleEvent::addHandle()
{
	if (!this->hasHandle())
	{
		sm_rwmutHandle.lockForWrite();
		sm_mapHandle.insert(m_eventHandleKey, this);
		sm_rwmutHandle.unlock();
	}
}

void HandleEvent::delHandle()
{
	if (this->hasHandle())
	{
		sm_rwmutHandle.lockForWrite();
		sm_mapHandle.erase(sm_mapHandle.find(m_eventHandleKey));
		sm_rwmutHandle.unlock();
	}
}

bool HandleEvent::hasHandle()
{
	sm_rwmutHandle.lockForRead();
	const auto res = (sm_mapHandle.find(m_eventHandleKey) != sm_mapHandle.end());
	sm_rwmutHandle.unlock();
	return res;
}

HandleEvent::Result HandleEvent::_handle(const QString& data)
{
	const auto nPos = data.indexOf(sm_commentChar);
	if (nPos == -1) { return Result::ECode::NotFoundSeparator; }

	const auto key = data.mid(0, nPos);
	const auto value = data.mid(nPos + 1);
	sm_rwmutHandle.lockForRead();
	auto iter = sm_mapHandle.find(key);
	if (iter == sm_mapHandle.end())
	{
		sm_rwmutHandle.unlock();
		return Result::ECode::NotRegEvent;
	}
	const auto res = iter.value()->_handle(value);
	sm_rwmutHandle.unlock();
	return res;
}

QString HandleEvent::handle(const QString& data)
{
	const auto res = this->_handle(data);
	QJsonObject obj;
	obj.insert("code", res.code);
	obj.insert("context", res.content);
	obj.insert("message", res.describe());
	return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}
