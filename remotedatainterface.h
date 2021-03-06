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

#ifndef REMOTEDATAINTERFACE_H
#define REMOTEDATAINTERFACE_H

#include <QObject>
#include <QList>
#include <QString>

//Good means the request was good and
//Fail means the remote service replied, but did not like the request, for some reason
//No Connect means that the request did not get thru to the remote service at all
enum class RequestState {FAIL, GOOD, NO_CONNECT};
//If RemoteDataReply returned is NULL, then the request was invalid due to internal error

enum class LongRunningState {INIT, PENDING, RUNNING, DONE, ERROR, PURGING}; //Add more if needed

class FileMetaData;

class LongRunningTask : public QObject
{
    Q_OBJECT

public:
    LongRunningTask(QObject * parent);

    virtual void cancelTask() = 0;
    virtual void purgeTaskData() = 0;
    virtual LongRunningState getState() = 0;

    virtual QString getIDstr() = 0;
    virtual QString getRawDataStr() = 0;

    //If one needs to know the parameters passed to the initial task,
    //find them returned here:
    virtual QMultiMap<QString, QString> * getTaskParamList() = 0;
    //The object returned here is destroyed with the LongRunningTask

signals:
    void stateChange(LongRunningState oldState, LongRunningState newState);

};

class RemoteDataReply : public QObject
{
    Q_OBJECT

public:
    RemoteDataReply(QObject * parent);

    virtual LongRunningTask * getLongRunningRef(bool claimRef = true) = 0;

    //If one needs to know the parameters passed to the initial task,
    //find them returned here:
    virtual QMultiMap<QString, QString> * getTaskParamList() = 0;
    //The object returned here is destroyed with the RemoteDataReply, unless
    //there is a LongRunningTask for this request.

signals:
    //All referenced values should be copied by the reciever or they will be discarded
    void haveCurrentRemoteDir(RequestState cmdReply, QString * pwd);
    void connectionsClosed(RequestState cmdReply);

    void haveAuthReply(RequestState authReply);
    void haveLSReply(RequestState cmdReply, QList<FileMetaData> * fileDataList);

    void haveDeleteReply(RequestState replyState);
    void haveMoveReply(RequestState authReply, FileMetaData * revisedFileData);
    void haveCopyReply(RequestState authReply, FileMetaData * newFileData);
    void haveRenameReply(RequestState replyState, FileMetaData * newFileData);

    void haveMkdirReply(RequestState authReply, FileMetaData * newFolderData);

    void haveUploadReply(RequestState authReply, FileMetaData * newFileData);
    void haveDownloadReply(RequestState authReply);
    void haveBufferDownloadReply(RequestState authReply, QByteArray * fileBuffer);

    //Job replys should be in an intelligble format, JSON is used by Agave and AWS for various things
    void haveJobReply(RequestState authReply, QJsonDocument * rawJobReply);
};

class RemoteDataInterface : public QObject
{
    Q_OBJECT

public:
    RemoteDataInterface(QObject * parent);

    //Defaults to directory root,
    //Subsequent commands with remote folder names are either absolute paths
    //or reletive to the current working directory
    virtual RemoteDataReply * setCurrentRemoteWorkingDirectory(QString cd) = 0;
    virtual RemoteDataReply * closeAllConnections() = 0;

    //Remote tasks to be implemented in subclasses:
    //Returns a RemoteDataReply, which should have the correct signal attached to an appropriate slot
    virtual RemoteDataReply * performAuth(QString uname, QString passwd) = 0;

    virtual RemoteDataReply * remoteLS(QString dirPath) = 0;

    virtual RemoteDataReply * deleteFile(QString toDelete) = 0;
    virtual RemoteDataReply * moveFile(QString from, QString to) = 0;
    virtual RemoteDataReply * copyFile(QString from, QString to) = 0;
    virtual RemoteDataReply * renameFile(QString fullName, QString newName) = 0;

    virtual RemoteDataReply * mkRemoteDir(QString location, QString newName) = 0;

    virtual RemoteDataReply * uploadFile(QString location, QString localFileName) = 0;
    virtual RemoteDataReply * uploadBuffer(QString location, QByteArray fileData) = 0;
    virtual RemoteDataReply * downloadFile(QString localDest, QString remoteName) = 0;
    virtual RemoteDataReply * downloadBuffer(QString remoteName) = 0;

    virtual RemoteDataReply * runRemoteJob(QString jobName, QMultiMap<QString, QString> jobParameters, QString remoteWorkingDir) = 0;

    virtual void forceRefreshOfLongTasks() = 0;
    virtual QList<LongRunningTask *> getListOfLongTasks() = 0;
    virtual LongRunningTask * getLongTaskByRef(QString IDstr) = 0;

signals:
    void sendFatalErrorMessage(QString errorText);
    void longRunningTasksUpdated();
};

#endif // REMOTEDATAINTERFACE_H
