This is a _very_ simple countdown application which will automatically 
logout a user after a given period of time.

A big timer is shown with a clock counting down until the end of the session.
The digits will slowly turn red once the end of the session approaches.

5 minutes before the session ends a warning will be shown to the user. Once
the timer hits zero the session is ended. How this is done depends on the
session type: if the GNOME or Xfce session manager is running they will
asked to logout the current user. If this fails, or no (known) session manager
is running all processes for the user will be send a TERM signal.

