/****************************************************************************
**
** Copyright (C) 2017 RoundCubes UG.
** Contact: https://roundcubes.de
**
** This file is part of the Pluginsystem of AnzeigenChef.
**
** Add this interface to your Pluginproject
****************************************************************************/
#ifndef SEARCHAGENTINTERFACE_H
#define SEARCHAGENTINTERFACE_H

#include <QtCore>

enum PriceType
{
    Negotiating = 1, // VB @eBay
    FixPrice = 0,
    GiveAway = 2,
    Trade = 3, // Only markt.de
    BestPrice = 4 // Only markt.de
};

class SearchResult
{
public:
    QString AdId;
    QString AdTitle;
    QString AdDescription;
    QString AdSeoUrl;
    QDateTime AdStart;
    QDateTime AdEnd;
    QString AdImageUrl;
    QString AdDistance;
    int AdPrice = 0;
    PriceType AdPriceType = PriceType::Negotiating;
};

class SearchAgentInterface
{
public:
    virtual ~SearchAgentInterface() {}
    virtual QList<SearchResult> Search(const QUrl& rUrl, int rReadpages) = 0;
    virtual QString GetPlatformName() = 0;
    virtual QColor GetPlatformColor() = 0;
    virtual QString GetPlatformHash() = 0;
    virtual QString GetLastError() = 0;
    virtual QString GetCustomerHelpMessage() = 0;
    virtual QString GetPlatformLetters() = 0;
    virtual bool SendQuestionToAdOwner(const QString &accountUsername, const QString &accountPassword, const QString &myName, const QString &myPhone, const QString &advertId) = 0;
};

#define SearchAgentInterface_iid "MG.AnzeigenChef.SearchAgentInterface"

Q_DECLARE_INTERFACE(SearchAgentInterface, SearchAgentInterface_iid)

#endif // SEARCHAGENTINTERFACE_H
