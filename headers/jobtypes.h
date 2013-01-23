#ifndef JOBTYPE_H
#define JOBTYPE_H

#include <QObject>
#include <QMetaType>

class JobTypes: public QObject
{
    Q_OBJECT
    Q_ENUMS(JobType)

public:

    enum JobType
    {
        ECB_ENC = 0x01,
        CBC_ENC = 0x02,
        ECB_DEC = 0x04,
        CBC_DEC = 0x08,
    };
};

Q_DECLARE_METATYPE(JobTypes::JobType);

#endif // JOBTYPE_H
