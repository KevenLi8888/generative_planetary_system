#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;

    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(realtime, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    QFont section_font;
    section_font.setPointSize(16);
    section_font.setBold(true);

    QLabel *GPS_label = new QLabel(); // Final Project label
    GPS_label->setText("Generative Planetary Systems");
    GPS_label->setFont(section_font);
    QLabel *GPS_features_label = new QLabel();
    GPS_features_label->setText("Features");
    GPS_features_label->setFont(section_font);
    QLabel *GPS_params_label = new QLabel();
    GPS_params_label->setText("Procedural Parameters");
    GPS_params_label->setFont(section_font);
    QLabel *num_planet_label = new QLabel();
    num_planet_label->setText("Number of Planets");
    num_planet_label->setFont(font);
    QLabel *tesselation_label = new QLabel(); // Parameters label
    tesselation_label->setText("Tesselation");
    tesselation_label->setFont(font);
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);
    QLabel *filters_label = new QLabel(); // Filters label
    filters_label->setText("Filters");
    filters_label->setFont(font);
    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Parameter 1:");
    QLabel *param2_label = new QLabel(); // Parameter 2 label
    param2_label->setText("Parameter 2:");
    QLabel *near_label = new QLabel(); // Near plane label
    near_label->setText("Near Plane:");
    QLabel *far_label = new QLabel(); // Far plane label
    far_label->setText("Far Plane:");

    // Creates the boxes containing the parameter sliders and number boxes
    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l1 = new QHBoxLayout();
    QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
    QHBoxLayout *l2 = new QHBoxLayout();

    // Create slider controls to control parameters
    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    p1Slider->setTickInterval(1);
    p1Slider->setMinimum(1);
    p1Slider->setMaximum(25);
    p1Slider->setValue(1);

    p1Box = new QSpinBox();
    p1Box->setMinimum(1);
    p1Box->setMaximum(25);
    p1Box->setSingleStep(1);
    p1Box->setValue(1);

    p2Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 2 slider
    p2Slider->setTickInterval(1);
    p2Slider->setMinimum(1);
    p2Slider->setMaximum(25);
    p2Slider->setValue(1);

    p2Box = new QSpinBox();
    p2Box->setMinimum(1);
    p2Box->setMaximum(25);
    p2Box->setSingleStep(1);
    p2Box->setValue(1);

    // Adds the slider and number box to the parameter layouts
    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);

    l2->addWidget(p2Slider);
    l2->addWidget(p2Box);
    p2Layout->setLayout(l2);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    nearSlider->setTickInterval(1);
    nearSlider->setMinimum(1);
    nearSlider->setMaximum(1000);
    nearSlider->setValue(10);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.01f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);

    farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    farSlider->setTickInterval(1);
    farSlider->setMinimum(1000);
    farSlider->setMaximum(10000);
    farSlider->setValue(10000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(100.f);
    farBox->setSingleStep(0.1f);
    farBox->setValue(100.f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    // Create checkbox for filters
    filter1 = new QCheckBox();
    filter1->setText(QStringLiteral("Per-Pixel Filter (Invert)"));
    filter1->setChecked(false);

    filter2 = new QCheckBox();
    filter2->setText(QStringLiteral("Per-Pixel Filter (Chromatic)"));
    filter2->setChecked(false);

    filter3 = new QCheckBox();
    filter3->setText(QStringLiteral("Per-Pixel Filter (Grayscale)"));
    filter3->setChecked(false);

    filter4 = new QCheckBox();
    filter4->setText(QStringLiteral("Kernel-Based Filter (Blur)"));
    filter4->setChecked(false);

    filter5 = new QCheckBox();
    filter5->setText(QStringLiteral("Kernel-Based Filter (Sharpen)"));
    filter5->setChecked(false);

    filter6 = new QCheckBox();
    filter6->setText(QStringLiteral("Kernel-Based Filter (Emboss)"));
    filter6->setChecked(false);

    // Final Project:
    demo = new QPushButton();
    demo->setText(QStringLiteral("Solar System"));

    procedural = new QPushButton();
    procedural->setText(QStringLiteral("Procedural System"));

    pause = new QPushButton();
    pause->setText(QStringLiteral("Pause"));

    showOrbits = new QCheckBox();
    showOrbits->setText(QStringLiteral("Show Orbits"));
    showOrbits->setChecked(true);

    orbitCamera = new QCheckBox();
    orbitCamera->setText(QStringLiteral("Use Orbit Camera"));
    orbitCamera->setChecked(false);

    proceduralTexture = new QCheckBox();
    proceduralTexture->setText(QStringLiteral("Use Procedural Texture"));
    proceduralTexture->setChecked(false);

    normalMapping = new QCheckBox();
    normalMapping->setText(QStringLiteral("Enable Normal Mapping"));
    normalMapping->setChecked(false);

    QGroupBox *g1Layout = new QGroupBox();
    QHBoxLayout *g1 = new QHBoxLayout();

    numPlanetSlider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    numPlanetSlider->setTickInterval(1);
    numPlanetSlider->setMinimum(1);
    numPlanetSlider->setMaximum(12);
    numPlanetSlider->setValue(1);

    numPlanetBox = new QSpinBox();
    numPlanetBox->setMinimum(1);
    numPlanetBox->setMaximum(12);
    numPlanetBox->setSingleStep(1);
    numPlanetBox->setValue(1);

    g1->addWidget(numPlanetSlider);
    g1->addWidget(numPlanetBox);
    g1Layout->setLayout(g1);

    vLayout->addWidget(GPS_label);
    vLayout->addWidget(demo);
    vLayout->addWidget(procedural);
    vLayout->addWidget(pause);
    vLayout->addWidget(GPS_features_label);
    vLayout->addWidget(showOrbits);
    vLayout->addWidget(proceduralTexture);
    vLayout->addWidget(normalMapping);
    vLayout->addWidget(GPS_params_label);
    vLayout->addWidget(num_planet_label);
    vLayout->addWidget(g1Layout);
    vLayout->addWidget(tesselation_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(param2_label);
    vLayout->addWidget(p2Layout);
    vLayout->addWidget(camera_label);
    vLayout->addWidget(orbitCamera);
    vLayout->addWidget(near_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(far_label);
    vLayout->addWidget(farLayout);
    vLayout->addWidget(filters_label);
    vLayout->addWidget(filter1);
    vLayout->addWidget(filter2);
    vLayout->addWidget(filter3);
    vLayout->addWidget(filter4);
    vLayout->addWidget(filter5);
    vLayout->addWidget(filter6);

    connectUIElements();

    // Set default values of 5 for tesselation parameters
    onValChangeP1(25);
    onValChangeP2(25);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    onValChangeFarBox(100.f);

    // Set default values for GPS
    onValChangeG1(9);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectFilters();
    connectParam1();
    connectParam2();
    connectNear();
    connectFar();
    connectGPS();
    connectG1();
}

void MainWindow::connectFilters() {
    connect(filter1, &QCheckBox::clicked, this, &MainWindow::onFilter1);
    connect(filter2, &QCheckBox::clicked, this, &MainWindow::onFilter2);
    connect(filter3, &QCheckBox::clicked, this, &MainWindow::onFilter3);
    connect(filter4, &QCheckBox::clicked, this, &MainWindow::onFilter4);
    connect(filter5, &QCheckBox::clicked, this, &MainWindow::onFilter5);
    connect(filter6, &QCheckBox::clicked, this, &MainWindow::onFilter6);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}

void MainWindow::connectGPS() {
    connect(demo, &QPushButton::clicked, this, &MainWindow::onDemo);
    connect(procedural, &QPushButton::clicked, this, &MainWindow::onProcedural);
    connect(pause, &QPushButton::clicked, this, &MainWindow::onPause);
    connect(showOrbits, &QCheckBox::clicked, this, &MainWindow::onShowOrbits);
    connect(orbitCamera, &QCheckBox::clicked, this, &MainWindow::onOrbitCamera);
    connect(proceduralTexture, &QCheckBox::clicked, this, &MainWindow::onProceduralTexture);
    connect(normalMapping, &QCheckBox::clicked, this, &MainWindow::onNormalMapping);
}

void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

// Filters
void MainWindow::onFilter1() {
    settings.filter1 = !settings.filter1;
}

void MainWindow::onFilter2() {
    settings.filter2 = !settings.filter2;
}

void MainWindow::onFilter3() {
    settings.filter3 = !settings.filter3;
}

void MainWindow::onFilter4() {
    settings.filter4 = !settings.filter4;
}

void MainWindow::onFilter5() {
    settings.filter5 = !settings.filter5;
}

void MainWindow::onFilter6() {
    settings.filter6 = !settings.filter6;
}

void MainWindow::onDemo() {
    settings.procedural = false;
    realtime->sceneChanged();
}

void MainWindow::onProcedural() {
    settings.procedural = true;
    realtime->sceneChanged();
}

void MainWindow::onPause() {
    settings.pause = !settings.pause;
}

void MainWindow::onShowOrbits() {
    settings.showOrbits = !settings.showOrbits;
}

void MainWindow::onOrbitCamera() {
    settings.orbitCamera = !settings.orbitCamera;
    realtime->settingsChanged();
}

void MainWindow::onProceduralTexture() {
    settings.proceduralTexture = !settings.proceduralTexture;
}

void MainWindow::onNormalMapping() {
    settings.normalMapping = !settings.normalMapping;
}

void MainWindow::onValChangeG1(int newValue) {
    numPlanetSlider->setValue(newValue);
    numPlanetBox->setValue(newValue);
    settings.numPlanet = numPlanetSlider->value();
}

void MainWindow::connectG1() {
    connect(numPlanetSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeG1);
    connect(numPlanetBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeG1);
}
