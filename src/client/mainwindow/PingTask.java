package mainwindow;

import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;

import java.io.IOException;
import java.util.Date;
import java.util.TimerTask;

public class  PingTask extends TimerTask {

    private SocketHandler s;
    private Circle ping_status;
    private int pendingResponses = 0;

    PingTask(SocketHandler s, Circle c) {
        this.s = s;
        ping_status = c;
    }

    @Override
    public void run() {
        System.out.println(new Date() + "-- ping 12345");
        try {
            s.sendMessage("ping 12345");
            pendingResponses++;
        } catch (Exception e) {
            System.err.println("Could not ping remote host.");
            s.setConnected(false);
            ping_status.setFill(Color.RED);
        }
        if(pendingResponses >= 2) {
            ping_status.setFill(Color.ORANGE);
            System.err.println("Remote Host did not answer. pending responses : " + pendingResponses);
        }
    }

    public void notifyPongResponse(){
        pendingResponses=0;
        s.setConnected(true);
        ping_status.setFill(Color.GREENYELLOW);
    }
}
