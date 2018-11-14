#ifndef HANDELSREGISTERAGENTPLUGIN_H
#define HANDELSREGISTERAGENTPLUGIN_H

#include <QGenericPlugin>
#include <QNetworkAccessManager>
#include "searchagentinterface.h"


class HandelsregisterAgentPlugin : public QObject, public SearchAgentInterface
{
    Q_OBJECT
    Q_INTERFACES(SearchAgentInterface)
    Q_PLUGIN_METADATA(IID "MG.AnzeigenChef.SearchAgentInterface" FILE "HandelsregisterAgent.json")

private:
    QString userAgent = "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) AppleWebKit/534.20 (KHTML, like Gecko) Chrome/11.0.672.2 Safari/534.20";
    QString pLastError;
    QString GetPartOfString(const QString &sourceString, const QString &fromString, const QString &toString);
    QString GetHtmlSourceCode(const QString& url, const QUrlQuery& post);
    QString FixHtml(const QString& fromString);

public:
    HandelsregisterAgentPlugin(QObject *parent = nullptr);
    QList<SearchResult> Search(const QUrl& url, int readpages) override;
    QString GetPlatformName() override;
    QString GetPlatformHash() override;
    QString GetLastError() override;
    QString GetCustomerHelpMessage() override;
    QColor GetPlatformColor() override;
    QString GetPlatformLetters() override;
    bool SendQuestionToAdOwner(const QString &accountUsername, const QString &accountPassword, const QString &myName, const QString &myPhone, const QString &advertId) override;
};

#endif // HANDELSREGISTERAGENTPLUGIN_H
