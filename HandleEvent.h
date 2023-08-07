#pragma once

#include <qstring.h>
#include <qmap>
#include <qreadwritelock.h>
#pragma execution_character_set("utf-8")

class HandleEvent
{
public:
	explicit HandleEvent() = default;
	HandleEvent(const QString& eventKey);
	~HandleEvent();

	void addHandle();
	void delHandle();
	bool hasHandle();

	QString handle(const QString& data);

public:
	class Result
	{
	public:
		enum ECode : int
		{
			Fail = -1, Succ = 0,
			NotRegEvent, NotFoundSeparator
		} code;
		QString content;

		Result(const ECode& _code = ECode::Fail, const QString& _content = "");

		inline QString describe() const
		{
			const auto iter = sm_mapCodeDesc.find(code);
			return (iter == sm_mapCodeDesc.end()) ? "描述信息未映射" : iter.value();
		}

	private:
		static const QMap<ECode, QString> sm_mapCodeDesc;
	};

protected:
	virtual Result _handle(const QString& data);

private:
	const QString m_eventHandleKey;

private:
	static QMap<QString, HandleEvent*> sm_mapHandle;
	static const QChar sm_commentChar;
	static QReadWriteLock sm_rwmutHandle;
};
