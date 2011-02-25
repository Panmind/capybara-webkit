#include "Connection.h"
#include "Visit.h"
#include "Find.h"
#include "Command.h"
#include "Reset.h"
#include "Attribute.h"

#include <QTcpSocket>
#include <iostream>

Connection::Connection(QTcpSocket *socket, WebPage *page, QObject *parent) :
    QObject(parent) {
  m_socket = socket;
  m_page = page;
  m_command = NULL;
  connect(m_socket, SIGNAL(readyRead()), this, SLOT(checkNext()));
}

void Connection::checkNext() {
  while (m_socket->canReadLine()) {
    readNext();
  }
}

void Connection::readNext() {
  char buffer[1024];
  qint64 lineLength = m_socket->readLine(buffer, 1024);
  if (lineLength != -1) {
    buffer[lineLength - 1] = 0;
    if (m_command) {
      m_command->receivedArgument(buffer);
    } else {
      m_command = startCommand(buffer);
      if (m_command) {
        connect(m_command,
                SIGNAL(finished(bool, QString &)),
                this,
                SLOT(finishCommand(bool, QString &)));
        m_command->start();
      } else {
        m_socket->write("bad command\n");
      }
    }
  }
}

Command *Connection::startCommand(const char *name) {
  #include "find_command.h"

  return NULL;
}

void Connection::finishCommand(bool success, QString &response) {
  m_command->deleteLater();
  m_command = NULL;
  if (success) {
    m_socket->write("ok\n");
  } else {
    m_socket->write("failure\n");
  }
  QString responseLength = QString::number(response.size()) + "\n";
  m_socket->write(responseLength.toAscii());
  m_socket->write(response.toAscii());
}

