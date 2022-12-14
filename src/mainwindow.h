#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectParam2();
    void connectNear();
    void connectFar();
    void connectFilters();
    void connectGPS();
    void connectG1();

    Realtime *realtime;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;

    QCheckBox *filter1;
    QCheckBox *filter2;
    QCheckBox *filter3;
    QCheckBox *filter4;
    QCheckBox *filter5;
    QCheckBox *filter6;

    // Final Project
    QPushButton *demo;
    QPushButton *pause;
    QPushButton *procedural;
    QCheckBox *orbitCamera;
    QCheckBox *showOrbits;
    QCheckBox *proceduralTexture;
    QSlider *numPlanetSlider;
    QSpinBox *numPlanetBox;

private slots:
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);

    // Filters
    void onFilter1();
    void onFilter2();
    void onFilter3();
    void onFilter4();
    void onFilter5();
    void onFilter6();

    // Final Project
    void onDemo();
    void onProcedural();
    void onPause();
    void onOrbitCamera();
    void onShowOrbits();
    void onProceduralTexture();
    void onValChangeG1(int newValue);
};
