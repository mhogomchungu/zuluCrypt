/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "createfilethread.h"
#include "miscfunctions.h"

#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <iostream>
#include <QDir>
#include <cstdio>
#include "zulucrypt.h"

createFileThread::createFileThread(QString source,QString file,double size,int type)
{
	m_source = source ;
	m_file = file ;
	m_size = size ;
	m_type = type ;
}

void createFileThread::run()
{
	char data ;

	//QFile blocked when reading from /dev/random for some reason,
	//going back to C API for file access

	FILE * in = fopen( m_source.toAscii().data(),"r") ;
	FILE * out = fopen( m_file.toAscii().data(),"w") ;
	double i ;

	if( m_type == 0 ){
		for( i = 0 ; i < m_size ; i++){
			do{
				data = fgetc(in) ;
			}while( data < 32 || data > 126) ;

			fputc(data,out) ;
		}
	}else{
		for( i = 0 ; i < m_size ; i++){
			data = fgetc(in) ;
			fputc(data,out) ;
		}
	}
	fclose(in) ;
	fclose(out) ;
}
