package mainwindow;

import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Pane;
import javafx.util.Duration;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.Random;
import java.util.concurrent.ConcurrentLinkedQueue;


public class Fish extends Thread {

    public final static String PATH_TO_FISHES = "file:fishes/";

    private String name;
    private Timeline t;
    private ImageView i;
    private boolean displayed = false;
    private boolean started = false;
    private Position size;
    private Position start;
    private Position dim = new Position(0,0);

    private boolean isMoving = false;
    private int lastAddedTime=0;

    private Queue<KeyFrame> queue = new ConcurrentLinkedQueue<>();

    public Fish(String name, Position size, Position start) {
        this.name = name;
        this.size = size;
        this.start = start; //calculateCoordinatesFromPercentages(start);

        i = new ImageView(getImageFromName());
        i.setPreserveRatio(true);
    }

    public void display(Pane p) {
        dim.x = p.getWidth() - size.x;
        dim.y = p.getHeight() - size.y;

        if(size.x > size.y) {
            i.setFitHeight(p.getHeight() * size.y / 100);
        } else {
            i.setFitWidth(p.getWidth() * size.x / 100);
        }

        start.x = p.getWidth() * start.x / 100;
        start.y = p.getHeight() * start.y / 100;

        if(!displayed){
            displayed = true;
            i.setTranslateX(start.x);
            i.setTranslateY(start.y);
            p.getChildren().add(i);
        }
    }

    public void updatePath(Position dest, Position size, int time){
        Position tmp = calculateCoordinatesFromPercentages(dest);
        queue.add(new KeyFrame(Duration.seconds(time+lastAddedTime), new KeyValue(i.translateXProperty(), tmp.x)));
        queue.add(new KeyFrame(Duration.seconds(time+lastAddedTime), new KeyValue(i.translateYProperty(), tmp.y)));
        lastAddedTime = time;
        if(!isMoving){
            run();
        }
    }

    private void loadWaitingKeyFrames(){
        lastAddedTime = 0;
        for (KeyFrame k : queue){
            t.getKeyFrames().add(queue.poll());
        }
    }


    public void run(){
        t = null;
        t = new Timeline();
        if(queue.isEmpty()){
            isMoving = false;
            return;
        }
        t.setOnFinished(e -> run());
        loadWaitingKeyFrames();
        isMoving = true;
        t.play();
    }


    private Image getImageFromName() {
        String fishName = new File( "fishes/" + name.split("_")[0] + ".png").exists() ? name : "nemo";
        String imagePath = PATH_TO_FISHES + fishName + ".png";
        Image i = new Image(imagePath);
        return i;
    }

    private static List<String> getAvailableFishNames() throws Exception {
        File folder = new File("fishes");
        File[] listOfFiles = folder.listFiles();
        if(listOfFiles == null){
            throw new Exception("Fishes not found");
        }

        List<String> l = new ArrayList<String>();
        for(File f : listOfFiles){
            l.add(f.toString().split("/")[1].split(".png")[0]);
        }
        return l;
    }

    public static String getRandomFishName() throws Exception {
        Random r = new Random();
        List<String> l = getAvailableFishNames();
        return l.get(r.nextInt(l.size()));
    }

    public void hide(Pane p){
        p.getChildren().remove(i);
    }

    private Position calculateCoordinatesFromPercentages(Position pos){
        return new Position(pos.x * dim.x / 100, pos.y * dim.y / 100);
    }

    private String getFishState(){
        if(started)
            return "started";
        else
            return "notStarted";
    }

    public String getFishName(){
        return name;
    }

    @Override
    public String toString(){
        return "Fish " + name + " at " + start + "," + size + " " + getFishState();
    }
}
