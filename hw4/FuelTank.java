import java.util.concurrent.ThreadLocalRandom;

public class FuelTank implements Runnable {
  private final int nitroCap = 20;
  private final int quantumCap = 8;
  private final int nitrogenTank = 75;
  private final int quantumTank = 30;
  private final String id;
  private FuelStation station;
  private boolean type;

  /**
   * 
   * @param station - Fuel Station
   * @param id      - tanker id
   * @param type    - true=nitrogen, false=quantum
   */
  public FuelTank(FuelStation station, String id, boolean type) {
    this.station = station;
    this.id = id;
    this.type = type;
  }

  public void run() {
    ThreadLocalRandom rand = ThreadLocalRandom.current();
    int fuel, nitrogen, quantum;
    boolean coin;
    while (true) {
      // vehicle travelling
      try {
        Thread.sleep(rand.nextInt(10000, 15000));

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
  }
}
