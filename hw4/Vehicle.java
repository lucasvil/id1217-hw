import java.util.concurrent.ThreadLocalRandom;

public class Vehicle implements Runnable {
  private final int nitroCap = 20;
  private final int quantumCap = 8;
  private final int rounds;
  private final String id;
  private FuelStation station;

  public Vehicle(FuelStation station, String id, int rounds) {
    this.station = station;
    this.id = id;
    this.rounds = rounds;
  }

  public void run() {
    ThreadLocalRandom rand = ThreadLocalRandom.current();
    int nitrogen, quantum;
    boolean type;
    for (int i = 0; i < this.rounds; i++) {
      // vehicle travelling
      try {
        Thread.sleep(rand.nextInt(2000, 5000));
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
      // make sure always at least one fuel type is needed
      type = rand.nextBoolean();

      // get needed fuel
      nitrogen = rand.nextInt((type ? 1 : 0), nitroCap);
      quantum = rand.nextInt((type ? 0 : 1), quantumCap);

      try {
        // refuel vehicle...
        station.refuel(id, nitrogen, quantum);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    // vehicle is done
    System.out.println(this.id + " done!");
  }
}
