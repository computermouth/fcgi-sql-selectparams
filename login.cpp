/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include <stdio.h>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "fcgio.h"

using namespace std;

int main(void)
{
	streambuf * cin_streambuf = cin.rdbuf();
	streambuf * cout_streambuf = cout.rdbuf();
	streambuf * cerr_streambuf = cerr.rdbuf();

	FCGX_Request request;
	FCGX_Init();
	FCGX_InitRequest(&request, 0, 0);
	string queryString;
	
	while(FCGX_Accept_r(&request) == 0){
		fcgi_streambuf cin_fcgi_streambuf(request.in);
		fcgi_streambuf cout_fcgi_streambuf(request.out);
		fcgi_streambuf cerr_fcgi_streambuf(request.err);
		
		cin.rdbuf(&cin_fcgi_streambuf);
		cout.rdbuf(&cout_fcgi_streambuf);
		cerr.rdbuf(&cerr_fcgi_streambuf);
		
		cout << "Content-type: text/plain\r\n\r\n";
		
		queryString = FCGX_GetParam("QUERY_STRING", request.envp);

		cout << "QS: " << queryString << endl;
		
		unsigned short qsLen = queryString.length();
		char * input = new char[qsLen];
		queryString.copy(input, qsLen);
		
		string userN = strtok(input, "+");
		string passH = strtok(NULL, "+");
		
		cout << "\nPARAMS: \n\tUSER: " << userN << 
				"\n\tPASS: " << passH << endl;	

		delete input;

		try {
			sql::Driver *driver;
			sql::Connection *con;
			sql::Statement *stmt;
			sql::ResultSet *res;
			
			cout << "\nConnect to localhost:" << endl;
			cout << "\tCheck for: \n\t\t queried username '" << userN << "'";
			//cout << "\n\t\t with queried password '" << passH << "'";
			
			/* Create a connection */
			driver = get_driver_instance();
			con = driver->connect("localhost", "root", "root");
			//~ /* Connect to the MySQL test database */

			stmt = con->createStatement();
			stmt->execute("USE accounts");
			res = stmt->executeQuery("SELECT passhash FROM main WHERE id = 'uname'");
			while (res->next()){
				if (res->getString(1) == passH)
					cout << "\n\tVerified passH!";
				else
					cout << "\n\tIncorrect passH";
			}
			
			delete res;
			delete stmt;
			delete con;
		} catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}//end sql
	}//end fcgi

	cin.rdbuf(cin_streambuf);
	cout.rdbuf(cout_streambuf);
	cerr.rdbuf(cerr_streambuf);

	return EXIT_SUCCESS;
}
