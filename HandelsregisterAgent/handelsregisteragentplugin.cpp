#include "handelsregisteragentplugin.h"
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QColor>

typedef QPair<QByteArray, QByteArray> RawHeaderPair;

/**
 * @brief HandelsregisterAgentPlugin::HandelsregisterAgentPlugin
 * @param parent Parent is set by AnzeigenChef
 */
HandelsregisterAgentPlugin::HandelsregisterAgentPlugin(QObject *parent) { Q_UNUSED(parent); }

/**
 * @brief HandelsregisterAgentPlugin::Search
 * @param rUrl: Input url for parsing target site
 *
 * Overwrite SearchAgentInterface Search function
 *
 * @return QList<SearchResult>: List of SearchResult
 */
QList<SearchResult> HandelsregisterAgentPlugin::Search(const QUrl &url, int readpages)
{
    Q_UNUSED(url);
    Q_UNUSED(readpages);

    QList<SearchResult> resultList;

    QString currentUrl = "https://www.handelsregisterbekanntmachungen.de/de/index.php?aktion=suche"; // Fix URL

    if (qApp->property("AppDown").toInt() == 1)
        return resultList;

    QThread::msleep(1000);

    QUrlQuery postData;
    postData.addQueryItem("vt", QDateTime::currentDateTime().toString("dd")); // Von Tag
    postData.addQueryItem("vm", QDateTime::currentDateTime().toString("MM")); // Von Monat
    postData.addQueryItem("vj", QDateTime::currentDateTime().toString("yyyy")); // Von Jahr

    postData.addQueryItem("bt", "31"); // Bis Tag
    postData.addQueryItem("bm", "12"); // Bis Monat
    postData.addQueryItem("bj", QDateTime::currentDateTime().toString("yyyy")); // Bis Jahr

    postData.addQueryItem("anzv","alle"); // Anzahl
    postData.addQueryItem("order","4"); // Datum der Bekanntmachung
    postData.addQueryItem("land","bw"); // Baden-Württemberg
    postData.addQueryItem("gericht","B8536"); // Freiburg

    postData.addQueryItem("suchart", "uneingeschr");

    QString responseString = GetHtmlSourceCode(currentUrl, postData);

    if (qApp->property("AppDown").toInt() == 1)
        return resultList;

    int timeout = 1000;

    if (!responseString.contains("javascript:NeuFenster('rb_id="))
        qInfo() << "No results";

    while(responseString.contains("javascript:NeuFenster('rb_id="))
    {
        QString part = GetPartOfString(responseString,"javascript:NeuFenster('rb_id=", "</LI>");
        QString id = GetPartOfString(part,"", "'");

        SearchResult newSearchResult;
        newSearchResult.AdId = id;
        newSearchResult.AdDescription = FixHtml(GetPartOfString(part,"<ul>", "</ul>").replace("<br>", ", "));
        newSearchResult.AdTitle = FixHtml(GetPartOfString(part,">", "<br>").replace("<ul>", ", "));
        newSearchResult.AdTitle = GetPartOfString(newSearchResult.AdTitle, ", ","");
        // newSearchResult.AdEnd = QDateTime::currentDateTime();
        newSearchResult.AdStart = QDateTime::currentDateTime();
        newSearchResult.AdSeoUrl = "https://www.handelsregisterbekanntmachungen.de/de/skripte/hrb.php?rb_id=" + id;
        resultList.append(newSearchResult);

        responseString = GetPartOfString(responseString,"javascript:NeuFenster('rb_id=", "");
        timeout--;
        if (timeout < 0)
            break;
    }

    return resultList;
}

/**
 * @brief HandelsregisterAgentPlugin::GetPartOfString
 * @param rSourceString String to parse
 * @param rFromString Begin of part
 * @param rToString End of part
 * @return SubString of begin and end, return empty String if not found
 */
QString HandelsregisterAgentPlugin::GetPartOfString(const QString &sourceString, const QString &fromString, const QString &toString)
{
    QString src = sourceString;

    if (src.isEmpty())
        return "";

    int start = 0;
    if (fromString != "")
    {
        start = src.indexOf(fromString);
        if (start >= 0)
        {
            start += fromString.length();
            int end = src.length();
            if (toString != "")
            {
                end = src.indexOf(toString, start);
                if (end < 0) return "";
            }
            return src.mid(start, end - start);
        }
        else
        {
            return "";
        }
    }
    else
    {
        int end = src.length();
        if (toString != "")
        {
            end = src.indexOf(toString, start + fromString.length());
            if (end < 0) return "";
        }
        if (end - start < 0)
        {
            return "";
        }
        return src.mid(start, end - start);
    }
}

/**
 * @brief HandelsregisterAgentPlugin::GetHtmlSourceCode
 * @param rUrl Source URL
 * @param rPost Any Postfields if needed, else use GET
 * @return HTML Source Code
 */
QString HandelsregisterAgentPlugin::GetHtmlSourceCode(const QString &url, const QUrlQuery &post)
{
    QNetworkAccessManager manager;

    QUrl uri(url);
    QEventLoop eventLoop;

    QNetworkRequest request(uri);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QNetworkReply *reply;
    if (post.isEmpty())
        reply = manager.get(request);
    else
        reply = manager.post(request,post.toString(QUrl::FullyEncoded).toUtf8());
    eventLoop.exec();

    QString responseString = "";
    auto error_type = reply->error();
    if (error_type == QNetworkReply::NoError) {
        responseString = reply->readAll();
    } else {
        pLastError = reply->errorString();
    }

    delete reply;

    return responseString;
}

/**
 * @brief HandelsregisterAgentPlugin::FixHtml
 * @param rFromString String to clean HTML Tags
 * @return Clean String
 */
QString HandelsregisterAgentPlugin::FixHtml(const QString &fromString)
{
    QString returnString = fromString;
    returnString = returnString.replace("&euro;","€");
    returnString = returnString.replace("&uuml;","ü");
    returnString = returnString.replace("&Uuml;","Ü");
    returnString = returnString.replace("&auml;","ä");
    returnString = returnString.replace("&Auml;","Ä");
    returnString = returnString.replace("&ouml;","ö");
    returnString = returnString.replace("&Ouml;","Ö");
    returnString = returnString.replace("&amp;","&");
    returnString = returnString.replace("&quot;","\"");
    returnString = returnString.replace("&nbsp;"," ");
    returnString = returnString.replace("&#x27;","'");
    returnString = returnString.replace("&#x2F;","/");
    returnString = returnString.replace("&#034;","\"");
    return returnString;
}

/**
 * @brief HandelsregisterAgentPlugin::GetPlatformName
 * @return Platformname for UI
 */
QString HandelsregisterAgentPlugin::GetPlatformName()
{
    return "Handelsregister";
}

/**
 * @brief HandelsregisterAgentPlugin::GetPlatformHash
 * @return Unique plugin identifier, use a hash value max. 50 chars
 */
QString HandelsregisterAgentPlugin::GetPlatformHash()
{
    return "HandelsregisterSearchAgentPlugin";
}

/**
 * @brief HandelsregisterAgentPlugin::GetLastError
 * @return Error message, only if an error happened
 */
QString HandelsregisterAgentPlugin::GetLastError()
{
    return pLastError;
}

/**
 * @brief HandelsregisterAgentPlugin::GetCustomerHelpMessage
 * @return Helptext for UI
 */
QString HandelsregisterAgentPlugin::GetCustomerHelpMessage()
{
    // TODO MG/DM 14.11.2018: Wirklich? Bitte Funktionen machen, auch wenn die anderen nachgezogen werden müssen.
    QStringList infoText;
    infoText.append("NOURL"); // NOURL sorgt dafür, dass keine URL eingegeben werden muss
    infoText.append("NOIGNORE"); // NOIGNORE sorgt dafür, dass der Bereich Ignorieren nicht angezeigt wird
    infoText.append("<select name=\"land\" label=\"Bundesländer\"><option value=\"\">-- Alle Bundesländer --</option><option value=\"bw\">Baden-Württemberg</option><option value=\"by\">Bayern</option><option value=\"be\">Berlin</option><option value=\"br\">Brandenburg</option><option value=\"hb\">Bremen</option><option value=\"hh\">Hamburg</option><option value=\"he\">Hessen</option><option value=\"mv\">Mecklenburg-Vorpommern</option><option value=\"ni\">Niedersachsen</option><option value=\"nw\">Nordrhein-Westfalen</option><option value=\"rp\">Rheinland-Pfalz</option><option value=\"sl\">Saarland</option><option value=\"sn\">Sachsen</option><option value=\"st\">Sachsen-Anhalt</option><option value=\"sh\">Schleswig-Holstein</option><option value=\"th\">Thüringen</option></select>");
    return infoText.join("");
}

/**
 * @brief HandelsregisterAgentPlugin::GetPlatformColor
 * @return Platformlabel color
 */
QColor HandelsregisterAgentPlugin::GetPlatformColor()
{
    return QColor::fromRgb(71, 23, 70);
}

QString HandelsregisterAgentPlugin::GetPlatformLetters()
{
    return "hr";
}

/**
 * @brief HandelsregisterAgentPlugin::SendQuestionToAdOwner
 *
 * You must implement your own login and sending process
 *
 * @return true if the message was sent successfully
 */
bool HandelsregisterAgentPlugin::SendQuestionToAdOwner(const QString &accountUsername, const QString &accountPassword, const QString &myName, const QString &myPhone, const QString &advertId)
{
    Q_UNUSED(accountUsername);
    Q_UNUSED(accountPassword);
    Q_UNUSED(myName);
    Q_UNUSED(myPhone);
    Q_UNUSED(advertId);

    return false;
}
