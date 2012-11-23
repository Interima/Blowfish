#include "blowfish.h"
#include "blocks.h"
#include <QString>
#include <QDebug>

Blowfish::Blowfish()
{
    R = 16;
}

QString  Blowfish::name() { return QString("Blowfish"); }

QString Blowfish::author() { return QString("B.Schneier");}

int Blowfish::keyLength() { return 448; }

int Blowfish::blockLength() { return 64; }

int Blowfish::speedLevel() { return 6; }

int Blowfish::safetyLevel() { return 8; }

int Blowfish::types() {return static_cast<int>(JobTypes::ECB_ENC|JobTypes::ECB_DEC);}

bool Blowfish::init(JobTypes::JobType t, QString k,QString s)
{
    qDebug()<<"[Blowfish say:] Start plugin init";

    qDebug()<<"[Blowfish say:] Set type of plugin";

    // set type
    this->type = t;

    // if key is empty return
    if (k.isEmpty()) return false;

    qDebug()<<"[Blowfish say:] Set key of plugin";

    keyExpander(k);

    qDebug()<<"[Blowfish say:] Set special args of plugin";
    // remove all spaces
    s.remove(QChar(' '));

    qint16 ind=0;

    bool ok=true;
    // parse
    QString temp = s.left(ind+1);

    if (temp.left(2)=="R=") this->R = temp.remove(0,2).toInt(&ok);

    qDebug()<<"[Blowfish say:] End plugin init";

    return (ok);
}


quint32 F(quint32 X,quint32 **S)
{
    quint8 A,B,C,D;

    D= X & 0xFF;
    C= (X>>8) & 0xFF;
    B= (X>>16) & 0xFF;
    A= (X>>24) & 0xFF;

    return (((S[0][A] + S[1][B]) XOR S[2][C]) + S[3][D]);
}


void Blowfish::encode(char *in, char *out)
{
    quint32 A=0,B=0;

    for(quint8 i=0; i<4; i++) A = A<<8 | static_cast<quint8>(in[i]);
    for(quint8 i=0; i<4; i++) B = B<<8 | static_cast<quint8>(in[i+4]);

    for(quint8 i=0; i<R/2; i++)
    {
        A = A XOR K[2*i];
        B = F(A,S) XOR B;

        B = B XOR K[2*i+1];
        A = F(B,S) XOR A;
    }

    quint32 temp;
    temp=A; A=B; B=temp;

    B= B XOR K [R];
    A= A XOR K [R+1];

    out[0] = static_cast<char> ((A>>24) & 0xFF);
    out[1] = static_cast<char> ((A>>16) & 0xFF);
    out[2] = static_cast<char> ((A>> 8) & 0xFF);
    out[3] = static_cast<char> ((A    ) & 0xFF);

    out[4] = static_cast<char> ((B>>24) & 0xFF);
    out[5] = static_cast<char> ((B>>16) & 0xFF);
    out[6] = static_cast<char> ((B>> 8) & 0xFF);
    out[7] = static_cast<char> ((B    ) & 0xFF);

}

void Blowfish::decode(char *in, char *out)
{
    quint32 A=0,B=0;

    for(quint8 i=0; i<4; i++) A = A<<8 | static_cast<quint8>(in[i]);
    for(quint8 i=0; i<4; i++) B = B<<8 | static_cast<quint8>(in[i+4]);


    for(quint8 i=R/2; i>0; i--)
        {
            A = A XOR K[2*i+1];
            B = F(A,S) XOR B;

            B = B XOR K[2*i];
            A = F(B,S) XOR A;
        }

    quint32 temp;
    temp=A; A=B; B=temp;

    B = B XOR K[1];
    A = A XOR K[0];

    out[0] = static_cast<char> ((A>>24) & 0xFF);
    out[1] = static_cast<char> ((A>>16) & 0xFF);
    out[2] = static_cast<char> ((A>> 8) & 0xFF);
    out[3] = static_cast<char> ((A    ) & 0xFF);

    out[4] = static_cast<char> ((B>>24) & 0xFF);
    out[5] = static_cast<char> ((B>>16) & 0xFF);
    out[6] = static_cast<char> ((B>> 8) & 0xFF);
    out[7] = static_cast<char> ((B    ) & 0xFF);
}


void Blowfish::keyExpander(QString key)
{
    // create S blocks
    S = new quint32 * [4];
    for(quint8 i=0;i<4;i++) S[i]= new quint32 [256];

    // create keys
    K = new quint32 [R+2];

    // init S
    for( quint16 i=0; i<256; i++)
    {
        S[0][i] = SBlock1[i];
        S[1][i] = SBlock2[i];
        S[2][i] = SBlock3[i];
        S[3][i] = SBlock4[i];
    }

    // init K
    for (quint8 i=0; i<R+2; i++) K[i]=S[0][i];

    // key to keys extract
    quint16 j=0;

    for (quint8 i=0; i<R+2; i++)
    {
        quint32 temp = key.at(j++).unicode()<<16;
        if (j == key.length()) j=0;

        temp = temp | key.at(j++).unicode();
        if (j == key.length()) j=0;

        K[i]=K[i] XOR temp;
    }

    // encode keys
    char *in = new char [8];
    char *out = new char [8];

    for (quint8 i=0; i<8; i++) in[i]=0;

    for(quint8 i=0; i<R+2; i+=2)
    {
        encode(in,out);

        quint32 A=0,B=0;

        for(quint8 j=0; j<4; j++) A = A<<8 | static_cast<quint8>(out[j]);
        for(quint8 j=0; j<4; j++) B = B<<8 | static_cast<quint8>(out[j+4]);

        K[i]= A;

        K[i+1]= B;

        // exchange
        for (quint8 j=0; j<8; j++) in[j]=out[j];
    }

    // encode S blocks
    for(quint8 i=0;i<4;i++)
        for(j=0;j<256;j+=2)
        {
            encode(in,out);

            quint32 A=0,B=0;

            for(quint8 m=0; m<4; m++) A = A<<8 | static_cast<quint8>(out[m]);
            for(quint8 m=0; m<4; m++) B = B<<8 | static_cast<quint8>(out[m+4]);

            S[i][j]= A;
            S[i][j+1]= B;

            // exchange
            for (quint8 m =0; m<8; m++) in[m]=out[m];
        }

    delete[] in;
    delete[] out;
}


PluginInterface* Blowfish::clone()
{
    return new Blowfish();
}

Q_EXPORT_PLUGIN2(Blowfish, Blowfish)
