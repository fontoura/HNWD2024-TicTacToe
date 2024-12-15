from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import urllib.parse
import os
import getpass
import platform
import pwd

PREFIX = '/upload/'

class RequestHandler(BaseHTTPRequestHandler):
    def _extract_file_path(self, path):
        # Extract the file name from the URL path
        if not path.startswith(PREFIX):
            return None
        return './' + urllib.parse.unquote(path[len(PREFIX):])

    def _get_username(self):
        # Try os.getlogin() (works if the process is associated with a terminal)
        try:
            return os.getlogin()
        except:
            pass

        # Try getpass.getuser() (cross-platform, usually reliable)
        try:
            return getpass.getuser()
        except:
            pass

        # Platform-specific environment variables
        if platform.system() == 'Windows':
            # Try USERNAME for Windows
            try:
                username = os.getenv("USERNAME")
                if username:
                    return username
            except:
                pass
        else:
            # Try USER or LOGNAME for Unix-like systems
            try:
                username = os.getenv("USER") or os.getenv("LOGNAME")
                if username:
                    return username
            except:
                pass

            # Try pwd.getpwuid (only on Unix-like systems)
            try:
                return pwd.getpwuid(os.getuid()).pw_name
            except:
                pass

        # If all methods fail, raise an error
        raise RuntimeError("Could not retrieve username")
    
    #def _list_directory(self, directory):
    #    # List the contents of a directory
    #    files = os.listdir(directory)
    #    file_list = [f'<li><a href="./{f}">{f}</a></li>' for f in files]
    #    return f'<h1>{directory}</h1><hr/><ul>{"".join(file_list)}</ul>'.encode()

    def do_POST(self):
            # Extract the file name from the URL path
            path = self.path
            file_name = self._extract_file_path(path)
            if file_name is None:
                self.send_response(404)
                self.end_headers()
                self.wfile.write(b'Nothing to see here')
                return
            
            # Handle the file upload
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)

            # Save the uploaded file
            try:
                if os.path.isdir(file_name):
                    self.send_response(400)
                    self.end_headers()
                    self.wfile.write(b'Cannot upload file: target is a directory')
                else:
                    with open(file_name, 'wb') as file:
                        file.write(post_data)
                    self.send_response(200)
                    self.end_headers()
                    self.wfile.write(f'File {file_name} uploaded successfully'.encode())
            except PermissionError:
                username = self._get_username()
                self.send_response(403)
                self.end_headers()
                self.wfile.write(f'Permission denied: user {username} cannot write file {file_name}'.encode())
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(f'Error saving file {file_name}: {e}'.encode())

    def do_GET(self):
            # Extract the file name from the URL path
            path = self.path
            file_name = self._extract_file_path(path)
            if file_name is None:
                self.send_response(404)
                self.end_headers()
                self.wfile.write(b'Nothing to see here')
                return

            # Read and serve the requested file
            try:
                if os.path.isdir(file_name):
                    self.send_response(400)
                    self.end_headers()
                    self.wfile.write(f'Cannot download file {file_name}: target is a directory'.encode())
                    return
                    #file_content = self._list_directory(file_name)
                    #file_type = 'text/html'
                else:
                    with open(file_name, 'rb') as file:
                        file_content = file.read()
                    file_type = 'text/plain'
                self.send_response(200)
                self.send_header('Content-type', file_type)
                self.end_headers()
                self.wfile.write(file_content)
            except FileNotFoundError:
                self.send_response(404)
                self.end_headers()
                self.wfile.write(f'File {file_name} not found'.encode())
            except PermissionError:
                username = self._get_username()
                self.send_response(403)
                self.end_headers()
                self.wfile.write(f'Permission denied: user {username} cannot read file {file_name}'.encode())
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(f'Error reading file {file_name}: {e}'.encode())

def run(server_class=ThreadingHTTPServer, handler_class=RequestHandler, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f'Starting server on port {port}...')
    httpd.serve_forever()

if __name__ == '__main__':
    run()
