#ifndef BLOWFISH_H
#define BLOWFISH_H

#include <QtCore>
#include "plugininterface.h"
#include "jobtypes.h"

#define XOR     ^
#define MUL32   *

class Blowfish : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:

    Blowfish();

    QString name();
    QString author();
    int keyLength();
    int blockLength();
    int speedLevel();
    int safetyLevel();
    int types();

    void encode (char *in, char* out);
    void decode (char *in, char* out);

    bool init(JobTypes::JobType type, QString key,QString args);

    PluginInterface* clone();

private:

    void keyExpander(QString key);
    JobTypes::JobType type;

    quint32 T;
    quint8  R;
    quint32 **S;
    quint32 *K;

};

#endif // BLOWFISH_H
