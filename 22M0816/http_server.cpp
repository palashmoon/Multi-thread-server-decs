#include "http_server.hh"
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <ctime>

vector<string> split(const string &s, char delim)
{
  vector<string> elems;

  stringstream ss(s);
  string item;

  while (getline(ss, item, delim))
  {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}

HTTP_Request::HTTP_Request(string request)
{

  vector<string> lines = split(request, '\n');

  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request

  /*
   TODO : extract the request method and URL from first_line here
  */

  vector<string> method_and_url = split(lines[0], ' ');

  /* finding the value of method and url here */
  this->method = method_and_url[0];
  this->url = method_and_url[1];

  if (this->method != "GET")
  {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req)
{

  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();

  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;
  struct dirent *dp;
  DIR *dir;
  string filename;

  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    string body;
    /*check if the file is index.html*/

    if (S_ISDIR(sb.st_mode))
    {

      /* In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */

      if ((dir = opendir(url.c_str())) == NULL)
      {
        perror("error while opening the directory");
      }

      else
      {
        while ((dp = readdir(dir)) != NULL)
        {
          filename = dp->d_name;

          if (filename == "index.html")
          {
            url = url + "/" + dp->d_name;
            break;
          }
        }
        if (filename != "index.html")
        {
          url = "html_files/error.html";
        }
      }

      closedir(dir);
    }
    /*
    TODO : open the file and read its contents
    */

    /*
    TODO : set the remaining fields of response appropriately
    */
    string line;
    fstream myfile;
    myfile.open(url);

    int length = 0;
    while (getline(myfile, line))
    {
      length = length + line.length();
      response->body.append(line);
    }
    response->content_length = to_string(length);
  }

  else
  {
    string line;
    fstream myfile;
    myfile.open("html_files/error.html");

    response->status_code = "404";

    response->status_text = "page not found";
    response->content_type = "text/html";

    int length = 0;
    while (getline(myfile, line))
    {
      length = length + line.length();
      response->body.append(line);
    }
    response->content_length = to_string(length);
  }

  delete request;

  return response;
}

string HTTP_Response::get_string()
{
  /*
  TODO : implement this function
  */

  time_t curr_time;
  curr_time = time(NULL);

  tm *tm_gmt = gmtime(&curr_time);

  int hour = tm_gmt->tm_hour;
  int min = tm_gmt->tm_min;
  int sec = tm_gmt->tm_sec;
  string result = "";
  string HTTP_version = "HTTP/1.0 ";
  string status_code = this->status_code + " ";
  string status_text = this->status_text + "\n";
  string date_time = "Date: Fri, 22 DEC 2019 " + to_string(hour) + ":" + to_string(min) + ":" + to_string(sec) + " " + "GMT" + "\n";
  string content_type = "Content-Type: " + this->content_type + "\n";
  string content_length = "Content-Length: " + this->content_length + "\r\n\n";
  string body = this->body;

  result = HTTP_version + status_code + status_text + date_time + content_type + content_length + body;
  // cout << result << endl;
  return result;
}
