import smtplib
import sys
import os
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email.utils import COMMASPACE, formatdate
from email import encoders

# python3 smtp.py mail.to.send@gmail.com filename message
if __name__ == "__main__":
    gmail_user = ''
    gmail_password = ''
    send_from = gmail_user  
    send_to = [sys.argv[1]]
    file_name = sys.argv[2]
    message_body = sys.argv[3]

    os.chdir('/home/neoisalie/Network/5/files')

    files = os.listdir()

    part = MIMEBase('application', "octet-stream")
    if(files.count(file_name) == 0):
        print('File not found')
    else:
        part.set_payload(open(file_name, "rb").read())
        encoders.encode_base64(part)
        part.add_header('Content-Disposition', 'attachment; filename="{}"'.format(file_name))

        msg = MIMEMultipart()
        msg['From'] = send_from
        msg['To'] = COMMASPACE.join(send_to)
        msg['Date'] = formatdate(localtime=True)
        msg['Subject'] = 'Message'
        msg.attach(MIMEText(message_body, 'plain'))
        msg.attach(part)
    
        try:
            server_ssl = smtplib.SMTP_SSL('smtp.gmail.com', 465)
            server_ssl.ehlo()
            server_ssl.login(gmail_user, gmail_password)
            server_ssl.ehlo()
            server_ssl.sendmail(send_from, send_to, msg.as_string())
            server_ssl.close()

            print('Email sent!')
        except Exception:
            print("Something went wrong!")
