#include <QCoreApplication>

#include "Hellow.h"

int main(int argc, char *argv[])
{
   QCoreApplication a(argc, argv);

   HelloMain();

   return a.exec();
}
