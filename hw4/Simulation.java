public class Simulation {
  private static final int nitroCap = 300;
  private static final int quantumCap = 120;
  private static final int slots = 4;
  private static final int numVehicles = 8;
  private static final int numTankers = 2;
  private static final int maxRounds = 10;

  public static void main(String[] args) {
    if (args.length < 1) {
      System.out.println("Usage: 'java Simulation <# vehicle rounds>'\nExiting...");
      System.exit(-1);
    }
    int rounds = (Integer.parseInt(args[0]) < maxRounds) ? Integer.parseInt(args[0]) : maxRounds;
    FuelStation station = new FuelStation(nitroCap, quantumCap, slots);
    boolean type = true;

    for (int i = 0; i < numVehicles; i++) {
      Vehicle vehicle = new Vehicle(station, ("vehicle " + i), rounds);
      new Thread(vehicle).start();
    }
    for (int i = 0; i < numTankers; i++) {
      FuelTank tanker = new FuelTank(station, ("tanker" + i), (rounds / 2), type);
      new Thread(tanker).start();
      type = !type;
    }
  }
}
