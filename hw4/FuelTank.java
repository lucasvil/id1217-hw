import java.util.concurrent.ThreadLocalRandom;

public class FuelTank implements Runnable {
  private final int nitroCap = 20;
  private final int quantumCap = 8;
  private final int nitrogenTank = 75;
  private final int quantumTank = 30;
  private final String id;
  private final int rounds;
  private FuelStation station;
  private boolean type;

  public FuelTank(FuelStation station, String id, int rounds, boolean type) {
    this.station = station;
    this.id = id;
    this.type = type;
    this.rounds = rounds;
  }

  public void run() {
    ThreadLocalRandom rand = ThreadLocalRandom.current();
    int fuel, nitrogen, quantum;
    boolean coin;
    for (int i = 0; i < this.rounds; i++) {
      // vehicle travelling
      try {
        Thread.sleep(rand.nextInt(5000, 10000));

        // make sure always at least one fuel type is needed
        coin = rand.nextBoolean();

        // get needed fuel
        fuel = coin ? this.nitrogenTank : this.quantumTank;
        nitrogen = rand.nextInt((type ? 1 : 0), nitroCap);
        quantum = rand.nextInt((coin ? 0 : 1), quantumCap);

        // check tanker type and deposit fuel
        if (type)
          station.refillNitrogenTank(id, this.nitrogenTank);
        else
          station.refillQuantumTank(id, this.quantumTank);

        // refuel vehicle...
        station.refuel(id, nitrogen, quantum);
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    System.out.println(this.id + " done!");
  }
}
