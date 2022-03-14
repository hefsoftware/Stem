QT -= gui
QT += core

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Engine/Filesystem/filesystemmap.cpp \
        Engine/Helper/arguments.cpp \
        Engine/Helper/iterable.cpp \
        Engine/Helper/iterator.cpp \
        Engine/Helper/scope.cpp \
        Engine/Helper/scopesstack.cpp \
        Engine/Helper/valuedata.cpp \
        Engine/Helper/valuedataarray.cpp \
        Engine/Helper/valuedatafunction.cpp \
        Engine/Helper/valuedatamap.cpp \
        Engine/Helper/valuedatanull.cpp \
        Engine/Helper/valuedatavariant.cpp \
        Engine/Message/qdebug.cpp \
        Engine/Tag/TagSet.cpp \
        Engine/Tag/tagblock.cpp \
        Engine/Tag/tagextends.cpp \
        Engine/Tag/tagif.cpp \
        Engine/Tag/tagimport_from.cpp \
        Engine/Tag/taginclude_embed.cpp \
        Engine/Tag/tagmacro.cpp \
        Engine/Tag/taguse.cpp \
        Engine/engine.cpp \
        Engine/filesystem.cpp \
        Engine/message.cpp \
        Engine/operator.cpp \
        Engine/runthread.cpp \
        Engine/value.cpp \
        Parser/Node/accessor.cpp \
        Parser/Node/array.cpp \
        Parser/Node/arrayget.cpp \
        Parser/Node/binaryop.cpp \
        Parser/Node/block.cpp \
        Parser/Node/call.cpp \
        Parser/Node/concatenate.cpp \
        Parser/Node/conditional.cpp \
        Parser/Node/constbool.cpp \
        Parser/Node/constdouble.cpp \
        Parser/Node/constint.cpp \
        Parser/Node/constnull.cpp \
        Parser/Node/embed.cpp \
        Parser/Node/extends.cpp \
        Parser/Node/filter.cpp \
        Parser/Node/functioncall.cpp \
        Parser/Node/hash.cpp \
        Parser/Node/import.cpp \
        Parser/Node/importas.cpp \
        Parser/Node/include.cpp \
        Parser/Node/list.cpp \
        Parser/Node/macro.cpp \
        Parser/Node/map.cpp \
        Parser/Node/mapstring.cpp \
        Parser/Node/memberaccess.cpp \
        Parser/Node/name.cpp \
        Parser/Node/node.cpp \
        Parser/Node/node1.cpp \
        Parser/Node/node2.cpp \
        Parser/Node/nodenamevalue.cpp \
        Parser/Node/print.cpp \
        Parser/Node/set.cpp \
        Parser/Node/text.cpp \
        Parser/Node/unaryop.cpp \
        Parser/Node/use.cpp \
        Parser/expressionparser.cpp \
        Parser/lexer.cpp \
        Parser/lexerenv.cpp \
        Parser/parser.cpp \
        Parser/token.cpp \
        Parser/tokenstream.cpp \
        Parser/trim.cpp \
        Template/template.cpp \
        Util/location.cpp \
        main.cpp \
        runtest.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Engine/Filesystem/filesystemmap.h \
    Engine/Helper/arguments.h \
    Engine/Helper/iterable.h \
    Engine/Helper/iterator.h \
    Engine/Helper/scope.h \
    Engine/Helper/scopesstack.h \
    Engine/Helper/valuedata.h \
    Engine/Helper/valuedataarray.h \
    Engine/Helper/valuedatafunction.h \
    Engine/Helper/valuedatamap.h \
    Engine/Helper/valuedatanull.h \
    Engine/Helper/valuedatavariant.h \
    Engine/Message/qdebug.h \
    Engine/Tag/tag.h \
    Engine/engine.h \
    Engine/filesystem.h \
    Engine/message.h \
    Engine/operator.h \
    Engine/runthread.h \
    Engine/value.h \
    Parser/Node/array.h \
    Parser/Node/accessor.h \
    Parser/Node/arrayget.h \
    Parser/Node/binaryop.h \
    Parser/Node/block.h \
    Parser/Node/call.h \
    Parser/Node/concatenate.h \
    Parser/Node/conditional.h \
    Parser/Node/constant.h \
    Parser/Node/constbool.h \
    Parser/Node/constdouble.h \
    Parser/Node/constint.h \
    Parser/Node/constnull.h \
    Parser/Node/embed.h \
    Parser/Node/extends.h \
    Parser/Node/filter.h \
    Parser/Node/functioncall.h \
    Parser/Node/hash.h \
    Parser/Node/import.h \
    Parser/Node/importas.h \
    Parser/Node/include.h \
    Parser/Node/list.h \
    Parser/Node/macro.h \
    Parser/Node/map.h \
    Parser/Node/mapstring.h \
    Parser/Node/memberaccess.h \
    Parser/Node/name.h \
    Parser/Node/node.h \
    Parser/Node/node1.h \
    Parser/Node/node2.h \
    Parser/Node/nodenamevalue.h \
    Parser/Node/print.h \
    Parser/Node/set.h \
    Parser/Node/text.h \
    Parser/Node/unaryop.h \
    Parser/Node/use.h \
    Parser/expressionparser.h \
    Parser/lexer.h \
    Parser/lexerenv.h \
    Parser/parser.h \
    Parser/token.h \
    Parser/tokenstream.h \
    Parser/trim.h \
    Template/template.h \
    Util/location.h \
    runtest.h
