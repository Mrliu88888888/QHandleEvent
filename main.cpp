#include "HandleEvent.h"
#include <qcoreapplication.h>
#include <qdebug.h>

class TestHandleEvent : public HandleEvent
{
public:
	explicit TestHandleEvent()
		: HandleEvent("t")
	{}

protected:
	virtual Result _handle(const QString& data)
	{
		qDebug() << data;
		return Result(Result::ECode::Succ, "{succ}");
	}
};

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	HandleEvent h;
	TestHandleEvent t;

	qDebug() << h.handle("t=hello world");
	qDebug() << h.handle("t");
	qDebug() << h.handle("tt=");

	return a.exec();
}
