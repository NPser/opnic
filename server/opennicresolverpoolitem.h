/*
 * Copyright (c) 2012 Mike Sharkey <michael_sharkey@firstclass.com>
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Mike Sharkey wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */
#ifndef OPENNICRESOLVERPOOLITEM_H
#define OPENNICRESOLVERPOOLITEM_H

#include <QObject>
#include <QHostAddress>
#include <QDateTime>
#include <QString>

#include "opennicresolvertestresult.h"

#define RESOLVER_SAMPLE_STORAGE_LIMIT		10			/* number of samples to remember for averaging */

class OpenNICResolverPoolItem : public QObject
{
	Q_OBJECT
	public:

		OpenNICResolverPoolItem(QObject *parent = 0);
		OpenNICResolverPoolItem(const OpenNICResolverPoolItem& other);
		virtual ~OpenNICResolverPoolItem();

		OpenNICResolverPoolItem&	copy(const OpenNICResolverPoolItem& other);
		QHostAddress				hostAddress()		{return mHostAddress;}
		int							testCount()			{return mTestCount;}
		int							replyCount()		{return mReplyCount;}
		int							failCount()			{return testCount()-replyCount();}
		int							lastLatency()		{return mLatencySamples.count() ? mLatencySamples.at(mLatencySamples.count()-1) : 0;}
		double						averageLatency();
		QDateTime					lastReply()			{return mLastReply;}
		QDateTime					lastTimeout()		{return mLastTimeout;}
		QString						lastFault()			{return mLastFault;}
		bool						alive()				{return lastReply() > lastTimeout();}

		OpenNICResolverPoolItem&	operator=(const OpenNICResolverPoolItem& other);
		bool						operator==(OpenNICResolverPoolItem &other);
		bool						operator>(OpenNICResolverPoolItem &other);
		bool						operator<(OpenNICResolverPoolItem &other);
		bool						operator>=(OpenNICResolverPoolItem &other);
		bool						operator<=(OpenNICResolverPoolItem &other);

		QString						toString();

	signals:
		void						latencyRequest(OpenNICResolverPoolItem* poolItem);

	public slots:
		void						latencyReply(OpenNICResolverTestResult result);
		void						clear();

	private:
		QHostAddress				mHostAddress;		/* host address wrapper */
		int							mTestCount;			/* number of tests conducted */
		int							mReplyCount;		/* number of replies received */
		QList<int>					mLatencySamples;	/* last latency samples (msec) */
		QDateTime					mLastReply;			/* the time of teh last reply */
		QDateTime					mLastTimeout;		/* the time of the last timeout */
		QString						mLastFault;			/* the last fault message */
};


#endif // OPENNICRESOLVERPOOLITEM_H
