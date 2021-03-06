/* Copyright (c) 2012 Stanisław Halik <sthalik@misaki.pl>

 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 */

#include <QPointF>
#include <QList>
#include "FunctionConfig.h"
#include <QtAlgorithms>
#include <windows.h>
#include <QtAlgorithms>
#include <QSettings>
#include <math.h>
#include <QPixmap>

void FunctionConfig::SharedInitialize(QString title) {
	_title = title;
	_points = QList<QPointF>();
	_mutex = CreateMutex(NULL, false, NULL);
	_data = 0;
	_size = 0;
	lastValueTracked = QPointF(0,0);
	_tracking_active = false;
}

FunctionConfig::FunctionConfig(QString title, QList<QPointF> points) {
	SharedInitialize(title);
	_points = points;
	reload();
}

//
// Constructor with only Title in arguments.
//
FunctionConfig::FunctionConfig(QString title) {
	SharedInitialize(title);
	reload();
}

//
// Calculate the value of the function, given the input 'x'.
// Used to draw the curve and, most importantly, to translate input to output.
// The return-value is also stored internally, so the Widget can show the current value, when the Tracker is running.
//
double FunctionConfig::getValue(double x) {
	int x2 = (int) (x * MEMOIZE_PRECISION);
	int x3 = (int) ceil(x * MEMOIZE_PRECISION);
	WaitForSingleObject(_mutex, INFINITE);
	double first = getValueInternal(x2);
	double second = getValueInternal(x3);

	double t = x - (int) x;
	double ret = first + (second - first) * t;

	lastValueTracked.setX(x);
	lastValueTracked.setY(ret);

	ReleaseMutex(_mutex);
	return ret;
}

//
// The return-value is also stored internally, so the Widget can show the current value, when the Tracker is running.
//
bool FunctionConfig::getLastPoint(QPointF& point ) {

	WaitForSingleObject(_mutex, INFINITE);
	point = lastValueTracked;
	ReleaseMutex(_mutex);
	
	return _tracking_active;
}

double FunctionConfig::getValueInternal(int x) {
	double sign = x < 0 ? -1 : 1;
	x = x < 0 ? -x : x;
	int idx = x;
	double ret;
	if (!_data)
		ret = 0;
	else if (_size == 0)
		ret = 0;
	else if (idx < 0)
		ret = 0;
	else if (idx < _size)
		ret = _data[idx];
	else
		ret = _data[_size - 1];
	return ret * sign;
}

static __inline QPointF ensureInBounds(QList<QPointF> points, int i) {
	int siz = points.size();
	if (siz == 0 || i < 0)
		return QPointF(0, 0);
	if (siz > i)
		return points[i];
	return points[siz - 1];
}

static bool sortFn(const QPointF& one, const QPointF& two) {
	return one.x() < two.x();
}

void FunctionConfig::reload() {
	double i;
	int j, k;

	_size = 0;

	if (_points.size())
		qStableSort(_points.begin(), _points.end(), sortFn);

	if (_data)
		delete _data;
	_data = NULL;
	if (_points.size()) {
		_data = new double[_size = MEMOIZE_PRECISION * (((int) (_points[_points.size() - 1]).x()) + 1)];

		for (k = 0; k < (int) (_points[0].x() * MEMOIZE_PRECISION); k++) {
			_data[k] = _points[0].y() * k / (int) (_points[0].x() * MEMOIZE_PRECISION);
		}

		j = 1;

		for (i = _points[0].x(), k = (int) (_points[0].x() * MEMOIZE_PRECISION);
			 k < _size;
			 k++, i += (1.0f / MEMOIZE_PRECISION))
		{
			while (j < _points.size() && _points[j].x() <= i)
				j++;
			QPointF a_ = ensureInBounds(_points, j - 2);
			QPointF b_ = ensureInBounds(_points, j - 1);
			QPointF c_ = ensureInBounds(_points, j);
			QPointF d_ = ensureInBounds(_points, j + 1);
			double a = a_.y();
			double b = b_.y();
			double c = c_.y();
			double d = d_.y();
			double t = (c_.x() - b_.x()) == 0 ? 0 : (i - b_.x()) / (c_.x() - b_.x());
			double val = a * ((-t + 2) * t - 1) * t * 0.5 +
				b * (((3 * t - 5) * t) * t + 2) * 0.5 +
				c * ((-3 * t + 4) * t + 1) * t * 0.5 +
				d * ((t - 1) * t * t) * 0.5;
			_data[k] = val;
		}
	}
}

FunctionConfig::~FunctionConfig() {
	CloseHandle(_mutex);
	if (_data)
		delete _data;
}

//
// Remove a Point from the Function.
// Used by the Widget.
//
void FunctionConfig::removePoint(int i) {
	WaitForSingleObject(_mutex, INFINITE);
	_points.removeAt(i);
	reload();
	ReleaseMutex(_mutex);
}

//
// Add a Point to the Function.
// Used by the Widget and by loadSettings.
//
void FunctionConfig::addPoint(QPointF pt) {
	WaitForSingleObject(_mutex, INFINITE);
	_points.append(pt);
	reload();
	ReleaseMutex(_mutex);
}

//
// Move a Function Point.
// Used by the Widget.
//
void FunctionConfig::movePoint(int idx, QPointF pt) {
	WaitForSingleObject(_mutex, INFINITE);
	_points[idx] = pt;
	reload();
	ReleaseMutex(_mutex);
}

//
// Return the List of Points.
// Used by the Widget.
//
QList<QPointF> FunctionConfig::getPoints() {
	QList<QPointF> ret;
	WaitForSingleObject(_mutex, INFINITE);
	for (int i = 0; i < _points.size(); i++) {
		ret.append(_points[i]);
	}
	ReleaseMutex(_mutex);
	return ret;
}

//
// Load the Points for the Function from the INI-file designated by settings.
// Settings for a specific Curve are loaded from their own Group in the INI-file.
//
void FunctionConfig::loadSettings(QSettings& settings) {
	QList<QPointF> points;
	settings.beginGroup(QString("Curves-%1").arg(_title));
	
	//
	// Read the number of points, that was last saved. Default to 2 Points, if key doesn't exist.
	//
	int max = settings.value("point-count", 2).toInt();
	for (int i = 0; i < max; i++) {
		points.append(QPointF(settings.value(QString("point-%1-x").arg(i), (i + 1) * 20.0).toDouble(),
							  settings.value(QString("point-%1-y").arg(i), (i + 1) * 80.0).toDouble()));
	}
	settings.endGroup();
	WaitForSingleObject(_mutex, INFINITE);
	_points = points;
	reload();
	ReleaseMutex(_mutex);
}

//
// Save the Points for the Function to the INI-file designated by settings.
// Settings for a specific Curve are saved in their own Group in the INI-file.
// The number of Points is also saved, to make loading more convenient.
//
void FunctionConfig::saveSettings(QSettings& settings) {
	WaitForSingleObject(_mutex, INFINITE);
	settings.beginGroup(QString("Curves-%1").arg(_title));
	int max = _points.size();
	settings.setValue("point-count", max);
	for (int i = 0; i < max; i++) {
		settings.setValue(QString("point-%1-x").arg(i), _points[i].x());
		settings.setValue(QString("point-%1-y").arg(i), _points[i].y());
	}
	settings.endGroup();
	ReleaseMutex(_mutex);
}
