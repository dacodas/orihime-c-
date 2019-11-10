1. Use `LocationMatch` with regular expression to match users to
   resource paths for authorization
   [https://httpd.apache.org/docs/2.4/en/mod/core.html#locationmatch]

2. Use `mod_auth_form` to take advantage of sessions, can provide
   multiple authentication backends to this. See if we can use
   `mod_auth_openidc` as an authentication provider for this
   
3. For now use `mod_authn_dbd` for the authentication, and see if 
