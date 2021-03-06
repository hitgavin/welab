/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2021, gavin.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#ifndef WELAB__WELAB__RESTART_HPP_
#define WELAB__WELAB__RESTART_HPP_

#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QStringList>

#include <qglobal.h>

class Restarter {
public:
  Restarter(int argc, char **argv) {
    Q_UNUSED(argc);
    executable_ = QString::fromLocal8Bit(argv[0]);
    working_path_ = QDir::currentPath();
  }

  void setArguments(const QStringList &args) { args_ = args; }

  QString executable() const { return executable_; }
  QStringList arguments() const { return args_; }
  QString workingPath() const { return working_path_; }

  int restartOrExit(int exit_code) { return qApp->property("restart").toBool() ? restart(exit_code) : exit_code; }

  int restart(int exit_code) {
    QProcess::startDetached(executable_, args_, working_path_);
    return exit_code;
  }

private:
  QString executable_;
  QStringList args_;
  QString working_path_;
};

#endif