#include "korrektswimpl.h"
#include "berechnungen.h"
#include "definitionen.h"
#include <math.h>
#include <QSettings>
#include "mainwindowimpl.h"
#include <QSettings>
#include <QString>
#include "errormessage.h"

#include <QSettings>
#include <QString>
#include <QTableWidgetItem>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QList>
#include <QBrush>
#include <time.h>
#include <QPrintPreviewDialog>
#include <QDebug>
#include <QStyleFactory>
#include <QUrl>
#include <QFileInfo>
#include <QProcess>
#include <QDesktopServices>



//
KorrektSwImpl::KorrektSwImpl( QWidget * parent, Qt::WindowFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	abgebrochen = false;
	RefraktometerInaktiv = false;
	SWAnstellen = 0;

	//Faktor für Umrechnung Brix nach Plato aus Konfigdatei auslesen
	LeseKonfig();

	//Verbinde Eingabefeld dichte mit Funktion zur Umrechnung in Grad Plato
	connect(spinBox_SwDichte, SIGNAL( valueChanged(double) ), this, SLOT( slot_SwDichteChanged(double) ));

	//Verbinde Eingabefeld GradPlato mit Funktion zur Umrechnung in Dichte
	connect(spinBox_SwPlato, SIGNAL( valueChanged(double) ), this, SLOT( slot_SwPlatoChanged(double) ));
	
	//Verbinde Eingabefeld GradBrix mit Funktion zur Umrechnung in Dichte
	connect(spinBox_SwBrix, SIGNAL( valueChanged(double) ), this, SLOT( slot_SwBrixChanged(double) ));



	//OK und Cancel Button mit funktion verbinden
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(MyAccept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(MyReject()));
}
//




void KorrektSwImpl::slot_SwDichteChanged(double value)
{
	if (spinBox_SwDichte -> hasFocus()){
		double sw;
		QBerechnungen ber;
		
		sw = ber.GetGradPlato(value);
		spinBox_SwPlato -> setValue(sw);
		sw = sw * FaktorBrixPlato;
		if (SWAnstellen == 0){
			spinBox_SwBrix -> setValue(sw);
		}
		else {
            //spinBox_SwBrix -> setValue(sw);
		}
	}
}


void KorrektSwImpl::slot_SwPlatoChanged(double value)
{
	if (spinBox_SwPlato -> hasFocus()){
		double d;
		QBerechnungen ber;
		
		d = ber.GetDichte(value);
		spinBox_SwDichte -> setValue(d);
		d = value * FaktorBrixPlato;
        //spinBox_SwBrix -> setValue(d);
		if (SWAnstellen == 0){
            //spinBox_SwBrix -> setValue(d);
		}
		else {
            //spinBox_SwBrix -> setValue(FormelBrixPlato -> value());
            doubleSpinBox_2 -> setValue((SWAnstellen * 1.0653 - spinBox_SwPlato -> value()) / 2);
		}
        //spinBox_SwBrix -> setValue(d);
	}
}

void KorrektSwImpl::slot_SwBrixChanged(double value)
{
	if (spinBox_SwBrix -> hasFocus()){
		double d;
		QBerechnungen ber;
        if (SWAnstellen == 0){
			//Berechnung ohne Alkoholeinfluss
			d = value / FaktorBrixPlato;

		}
		else {
			double sw = SWAnstellen;

			//Standardformel
			if (FormelBrixPlato == "Standardformel"){
				value = value / FaktorBrixPlato;
				sw = sw / FaktorBrixPlato;
				//Berechnung mit Alkoholeinfluss
				//Restextrakt = 1,001843 – 0,002318474 . BIa – 0.000007775 . BIa² 
				              //– 0,000000034 . BIa³ + 0,00574 . BIe + 0,00003344 
				              //. BIe² + 0,000000086 . BIe³ 
				d = 1.001843 - 0.002318474 * sw - 0.000007775 * pow(sw,2) 
						- 0.000000034 * pow(sw,3) + 0.00574 * value + 0.00003344 
						* pow(value,2) + 0.000000086 * pow(value,3);  
				d = ber.GetGradPlato(d);
			}
			//Variante 2 von User Kleier -> Quelle:  
			//http://hobbybrauer.de/modules.php?name=eBoard&file=viewthread&tid=11943&page=2#pid129201
			else {
				double Ballingkonstante = 2.0665;
				//tatsächlicher Restextrakt
				double tr =(Ballingkonstante * value - Gaerungskorrektur * sw)/(Ballingkonstante 
						* FaktorBrixPlato - Gaerungskorrektur);			
				//Scheinbarer Restextrakt
				d = tr * (1.22 + 0.001 * sw) - ((0.22 + 0.001 * sw) * sw);
			}
		}
		spinBox_SwPlato -> setValue(d);
		d = ber.GetDichte(d);
		spinBox_SwDichte -> setValue(d);
        doubleSpinBox_2 -> setValue((SWAnstellen * 1.0653 - spinBox_SwPlato -> value()) / 2);

	}
}

void KorrektSwImpl::BerDichte()
{
	double d;
	QBerechnungen ber;
	
	d = ber.GetDichte(spinBox_SwPlato -> value());
	spinBox_SwDichte -> setValue(d);
	if (SWAnstellen == 0){
		spinBox_SwBrix -> setValue(spinBox_SwPlato -> value() * FaktorBrixPlato);
	}
    else {
        spinBox_SwBrix -> setValue(FaktorBrixPlato * doubleSpinBox -> value());
	}
}


void KorrektSwImpl::MyAccept()
{
	abgebrochen = false;
	accept();
}


void KorrektSwImpl::MyReject()
{
	abgebrochen = true;
	reject();
}


void KorrektSwImpl::setRefraktometerInaktiv(bool value)
{
    RefraktometerInaktiv = value;
    label_BrixRefraktometer -> setVisible(!value);
	spinBox_SwBrix -> setVisible(!value);
	label_EinheitBrix -> setVisible(!value);
}


double KorrektSwImpl::getSWAnstellen()
{
	return SWAnstellen;
}

void KorrektSwImpl::setSWAnstellen(double value)
{
	SWAnstellen = value;
}

double KorrektSwImpl::getFaktorBrixPlato()
{
	return FaktorBrixPlato;
}


void KorrektSwImpl::setFaktorBrixPlato(double value)
{
	FaktorBrixPlato = value;
}


void KorrektSwImpl::LeseKonfig()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, KONFIG_ORDNER, APP_KONFIG);

	double d;
	settings.beginGroup("Erweitert");
	d = settings.value("FaktorBrixPlato").toDouble();
	
	//Default wert wenn Wert in Konfigdatei noch nicht vorhanden ist
	if (d == 0){
		d = 1.03;
	}
	FaktorBrixPlato = d;

	//Formel zur Umrechnung von Brix nach Plato im Gärverlauf
	FormelBrixPlato = settings.value("FormelBrixPlato").toString();

	//Gärungskorrektur
	d = settings.value("Gaerungskorrektur").toDouble();
	settings.endGroup();
	
	if (d == 0){
		d = 0.44552;
	}
    Gaerungskorrektur = d;
}



void KorrektSwImpl::on_doubleSpinBox_B_valueChanged(double arg1)
{

    spinBox_SwBrix -> setValue(arg1);
//spinBox_SwBrix -> setValue(arg1);
//doubleSpinBox -> setValue(arg1 / FaktorBrixPlato);

}

//void KorrektSwImpl::on_doubleSpinBox_valueChanged(double arg1)
//{
//spinBox_SwBrix -> setValue(arg1 * FaktorBrixPlato);
//doubleSpinBox_B -> setValue(arg1 * FaktorBrixPlato);

//}
void KorrektSwImpl::on_spinBox_SwBrix_valueChanged(double arg1)

{
doubleSpinBox -> setValue(arg1 / FaktorBrixPlato);
doubleSpinBox_B -> setValue(arg1);

}

//void KorrektSwImpl::on_doubleSpinBox(double value)
//{

    //doubleSpinBox -> Value(Berechnungen.BerAlkohoVol(sw,SWSchnellgaerprobe));


//    doubleSpinBox -> setValue(spinBox_AlkoholVol -> value());


//}

//void KorrektSwImpl::on_doubleSpinBox_2(double value)
//{
//    sw = spinBox_SWAnstellen -> value();
//    doubleSpinBox_2 -> Value(Berechnungen.GetScheinbarerEVG(sw,SWSchnellgaerprobe));



    //doubleSpinBox_2 -> setValue(Berechnungen.BerAlkohoVol(sw,SWSchnellgaerprobe));

//}

//Alkoholgehalt
//double sw = spinBox_SWAnstellen -> value();
//sw = sw + sw_ewz_gaerung;
//sw += (spinBox_HaushaltszuckerGesammt -> value() / 10) / spinBox_WuerzemengeAnstellen -> value();
//doubleSpinBox -> setValue(Berechnungen.BerAlkohoVol(sw,SWSchnellgaerprobe));

//Scheinbarer EVG
//sw = spinBox_SWAnstellen -> value();
//doubleSpinBox_2 -> setValue(Berechnungen.GetScheinbarerEVG(sw,SWSchnellgaerprobe));
