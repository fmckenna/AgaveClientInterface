/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#ifndef AGAVETASKREPLY_H
#define AGAVETASKREPLY_H

#include "../remotedatainterface.h"

#include <QtGlobal>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

enum class RequestState;
class AgaveHandler;
class AgaveTaskGuide;
class AgaveLongRunning;

class AgaveTaskReply : public RemoteDataReply
{
    Q_OBJECT
public:
    explicit AgaveTaskReply(AgaveTaskGuide * theGuide, QNetworkReply *newReply, AgaveHandler * theManager, QObject *parent = 0);
    ~AgaveTaskReply();

    virtual LongRunningTask * getLongRunningRef(bool claimRef = true);
    virtual QMultiMap<QString, QString> * getTaskParamList();

    //-------------------------------------------------
    //Agave specific:

    void invokePassThruReply();
    void delayedPassThruReply(RequestState replyState, QString * param1 = NULL);

    AgaveTaskGuide * getTaskGuide();

    static RequestState standardSuccessFailCheck(AgaveTaskGuide * taskGuide, QJsonDocument * parsedDoc);
    static FileMetaData parseJSONfileMetaData(QJsonObject fileNameValuePairs);

    static QJsonValue retriveMainAgaveJSON(QJsonDocument * parsedDoc, const char * oneKey);
    static QJsonValue retriveMainAgaveJSON(QJsonDocument * parsedDoc, QString oneKey);
    static QJsonValue retriveMainAgaveJSON(QJsonDocument * parsedDoc, QList<QString> keyList);
    static QJsonValue recursiveJSONdig(QJsonValue currObj, QList<QString> * keyList, int i);

signals:
    //For redirecting info to the Agave handler:
    void haveInternalTaskReply(AgaveTaskReply * theGuide, QNetworkReply * rawReply);

private slots:
    void rawTaskComplete();

private:
    void processNoContactReply(QString errorText);
    void processFailureReply(QString errorText);

    void processBadReply(RequestState replyState, QString errorText);

    AgaveHandler * myManager = NULL;
    AgaveTaskReply * passThruRef = NULL;
    AgaveTaskGuide * myGuide = NULL;
    QNetworkReply * myReplyObject = NULL;

    //PassThru reply store:
    RequestState pendingReply;
    QString pendingParam;

    QMultiMap<QString, QString> * taskParamList = NULL;
    bool longRunRefTaken = false;
    AgaveLongRunning * longRunRef = NULL;
};

#endif // AGAVETASKREPLY_H
