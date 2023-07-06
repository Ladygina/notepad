#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include"mainwindow.h"

#include <QSyntaxHighlighter>
#include<QXmlStreamReader>
#include<QXmlAttributes>
#include <QXmlStreamAttribute>
#include <QXmlStreamAttributes>
#include<QRegularExpression>
#include<QBrush>
#include<QColor>
#include<QFile>

struct HighlightingRule
{
    QRegularExpression pattern;
    QTextCharFormat format;
};


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

private:


    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat multiLineCommentFormat;


    public:

    Highlighter(QTextDocument *parent,QVector<HighlightingRule> highlightingRules);

    QXmlStreamReader xmlStream;

    QString fileExs;

    protected:
        void highlightBlock(const QString &text) override;

        friend class MainWindow;

};



#endif // HIGHLIGHTER_H
