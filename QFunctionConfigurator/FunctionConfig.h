/* Copyright (c) 2011-2012, Stanislaw Halik <sthalik@misaki.pl>

 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 */

#include <QList>
#include <QPointF>
#include <windows.h>
#include <QString>
#include <QSettings>
#include <QtDesigner/QDesignerExportWidget>

#ifndef FUNCTION_CONFIG_H
#define FUNCTION_CONFIG_H

#define MEMOIZE_PRECISION 200.0f

class QDESIGNER_WIDGET_EXPORT FunctionConfig {
private:
	HANDLE _mutex;
	QList<QPointF> _points;
	void reload();
	double* _data;
	int _size;
	QString _title;
	double getValueInternal(int x);
	QPointF lastValueTracked;								// The last input value requested by the Tracker, with it's output-value.
	bool _tracking_active;

	void SharedInitialize(QString title);

public:
	FunctionConfig(QString title, QList<QPointF> points);
	FunctionConfig(QString title);
	double getValue(double x);
	bool getLastPoint(QPointF& point);						// Get the last Point that was requested.
	virtual ~FunctionConfig();

	//
	// Functions to manipulate the Function
	//
	void removePoint(int i);
	void addPoint(QPointF pt);
	void movePoint(int idx, QPointF pt);
	QList<QPointF> getPoints();

	//
	// Functions to load/save the Function-Points to an INI-file
	//
	void saveSettings(QSettings& settings);
	void loadSettings(QSettings& settings);

	void setTrackingActive(bool blnActive) {
		_tracking_active = blnActive;
	}
	QString getTitle() { return _title; };
};

#endif