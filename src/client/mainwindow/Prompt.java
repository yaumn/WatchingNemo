package mainwindow;

import java.io.*;
import java.util.Scanner;


public class Prompt implements Runnable {

    private SocketHandler sh;
    private Command theCommand;
    private Scanner in;
    private PrintStream out;
    static public boolean statusAsked = false;

    Prompt(SocketHandler soc, Scanner s, PrintStream p){
	sh = soc;
	in = s;
	out = p;
    }

    private Command parse(String line){
	Command c;
	String[] lineParsed = line.split(" ");
	if (lineParsed[0].equals("status")) {
	    if (lineParsed.length != 1) {
		out.println("wrong usage : startFish [fishName]");
		return null;
	    }
	    statusAsked = true;
	    c = new Status();
	    return c;
	}
	else if (lineParsed[0].equals("startFish")) {
	    if (lineParsed.length != 2) {
		out.println("wrong usage : startFish [fishName]");
		return null;
	    }
	    c = new StartFish(lineParsed[1]);
	    return c;
	}
	else if (lineParsed[0].equals("delFish")) {
	    if (lineParsed.length != 2) {
		out.println("wrong usage : delFish [fishName]");
		return null;
	    }
	    c = new DelFish(lineParsed[1]);
	    return c;
	}
	else if (lineParsed[0].equals("addFish")) {
	    if (lineParsed.length != 6) {
		out.print("wrong usage : addFish [fishName] at [coordinate], [size], [mobility]\n");
		return null;
	    }
	    c = new AddFish(lineParsed[1], lineParsed[3].substring(0, lineParsed[3].length() - 1), lineParsed[4].substring(0, lineParsed[4].length() - 1), lineParsed[5]);
	    return c;
	}
	else if (line.contains("help")) {
	    print_help();
	    return null;
	}
	else {
	    out.println("NOK : Invalid command");
	    return null;
	}
    }

    private void print_help(){
	out.println("status : displays informations on the state of the application\n" +
		    "addFish <name> at 00x00, 00x00, <moving strategy> : adds a fish accordingly to the given parameters\n" +
		    "delFish <name> : removes specified fish\n" +
		    "startFish <name> : enables given fish to move");
    }

    private void execute(Command c) {
	try {
	    theCommand.execute(sh, out);
	} catch (Exception e) {
	    out.println("Err: Client is not connected to the server.");
	    e.printStackTrace();
	}
    }

    public void run() {
	out.print("$ ");
	while (true) {
	    String input = in.nextLine();
	    out.print("\t-> ");

	    if("q".equals(input)){
		out.println("Exit!");
		System.exit(0);
	    }

	    theCommand = parse(input);
	    if(theCommand != null){
		execute(theCommand);
	    } else {
		out.print("$ ");
	    }
	}
    }
}
