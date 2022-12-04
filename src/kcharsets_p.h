#ifndef KCHARSETS_P_H
#define KCHARSETS_P_H

#include "kcharsets.h"
#include "kusasciitextcodec.h"

#include <QHash>
#include <QTextCodec>

class KCharsetsPrivate
{
public:
    explicit KCharsetsPrivate(KCharsets *_kc)
        : usAsciiTextCodec{new KUsAsciiTextCodec}
    {
        kc = _kc;
        codecForNameDict.reserve(43);
    }

    bool isUsAsciiTextCodecRequest(const QByteArray &name) const;

    // Hash for the encoding names (sensitive case)
    QHash<QByteArray, QTextCodec *> codecForNameDict;
    KCharsets *kc;
    // Using own variant due to broken ICU-based Qt codec, see QTBUG-83081.
    // US-ASCII being an important one, but perhaps others also need their variant here?
    // The life-time management is handled by Qt itself by
    // auto-registration inside the QTextCodec constructor.
    QTextCodec *const usAsciiTextCodec;

    // Cache list so QStrings can be implicitly shared
    QList<QStringList> encodingsByScript;

    KCODECS_EXPORT QTextCodec *codecForName(const QString &n);
    KCODECS_EXPORT QTextCodec *codecForName(const QString &n, bool &ok);
    QTextCodec *codecForNameOrNull(const QByteArray &n);
};

#endif
