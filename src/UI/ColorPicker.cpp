#include "include/UI/ColorPicker.h"

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent) {
    // Utworzenie etykiet
    QLabel *labelR = new QLabel("R", this);
    QLabel *labelG = new QLabel("G", this);
    QLabel *labelB = new QLabel("B", this);

    // Utworzenie pól edycji
    rEdit = new QLineEdit(this);
    rEdit->setText(QString("128"));
    gEdit = new QLineEdit(this);
    gEdit->setText(QString("128"));
    bEdit = new QLineEdit(this);
    bEdit->setText(QString("128"));

    // Ustawienie walidatora akceptującego liczby od 0 do 255
    QIntValidator *validatorR = new QIntValidator(0, 255, this);
    QIntValidator *validatorG = new QIntValidator(0, 255, this);
    QIntValidator *validatorB = new QIntValidator(0, 255, this);
    rEdit->setValidator(validatorR);
    gEdit->setValidator(validatorG);
    bEdit->setValidator(validatorB);

    // Layout dla każdego wiersza (etykieta + pole edycji)
    QHBoxLayout *layoutR = new QHBoxLayout();
    layoutR->addWidget(labelR);
    layoutR->addWidget(rEdit);

    QHBoxLayout *layoutG = new QHBoxLayout();
    layoutG->addWidget(labelG);
    layoutG->addWidget(gEdit);

    QHBoxLayout *layoutB = new QHBoxLayout();
    layoutB->addWidget(labelB);
    layoutB->addWidget(bEdit);

    // Widget do podglądu koloru
    colorPreview = new ColorPreview(this);
    colorPreview->setMinimumSize(50, 50);

    // Główny layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(layoutR);
    mainLayout->addLayout(layoutG);
    mainLayout->addLayout(layoutB);
    mainLayout->addWidget(colorPreview);

    // Aktualizacja koloru przy każdej zmianie tekstu w polach edycji
    connect(rEdit, &QLineEdit::textChanged, this, &ColorPicker::updateColor);
    connect(gEdit, &QLineEdit::textChanged, this, &ColorPicker::updateColor);
    connect(bEdit, &QLineEdit::textChanged, this, &ColorPicker::updateColor);
}

void ColorPicker::setColor(const Color& color){
    this->color = color;

    bool oldBlockR = rEdit->blockSignals(true);
    bool oldBlockG = gEdit->blockSignals(true);
    bool oldBlockB = bEdit->blockSignals(true);

    rEdit->setText(QString("%1").arg(color.R));
    gEdit->setText(QString("%1").arg(color.G));
    bEdit->setText(QString("%1").arg(color.B));

    rEdit->blockSignals(oldBlockR);
    gEdit->blockSignals(oldBlockG);
    bEdit->blockSignals(oldBlockB);

    colorPreview->setColor( QColor(color.R, color.G, color.B, color.A) );
}

Color ColorPicker::getColor(){
    return this->color;
}


void ColorPicker::updateColor() {
    int r = rEdit->text().isEmpty() ? 0 : rEdit->text().toInt();
    int g = gEdit->text().isEmpty() ? 0 : gEdit->text().toInt();
    int b = bEdit->text().isEmpty() ? 0 : bEdit->text().toInt();
    color = Color(b,g,r,255);
    QColor qColor(r, g, b);
    colorPreview->setColor(qColor);

    emit colorChanged(color);
}
