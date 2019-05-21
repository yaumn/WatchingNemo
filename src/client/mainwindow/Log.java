package mainwindow;

import java.util.logging.*;
import java.io.IOException;

public class Log {

    private int levelOfLog;
    protected static Logger logger = Logger.getLogger("mainwindow.SocketHandler");

    public Log(int logs) {
	levelOfLog = logs;
    }

    public void logs() {
	Handler fh = null;
	try {
	    fh = new FileHandler("Client.log", true);
	} catch (IOException e) {
	    e.printStackTrace();
	}
	fh.setFormatter(new SimpleFormatter());
	logger.addHandler(fh);
    }

    public void createLogs(Level l , int lvl, String msg) {
	if (levelOfLog >= lvl) {
	    logger.log(l,msg);
	}
    }
}
