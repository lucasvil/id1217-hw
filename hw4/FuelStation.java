import java.util.concurrent.atomic.AtomicInteger;

public class FuelStation {
  private final Object parking = new Object();
  private final Object fuelTank = new Object();
  private final int quantumCap, nitrogenCap, slotsCap;
  private AtomicInteger quantumLevel, nitrogenLevel, occupiedSlots, reservedNitrogen, reservedQuantum;

  public FuelStation(int n, int q, int s) {
    this.nitrogenCap = n;
    this.quantumCap = q;
    this.slotsCap = s;
    this.nitrogenLevel = new AtomicInteger(n);
    this.quantumLevel = new AtomicInteger(q);
    this.occupiedSlots = new AtomicInteger(0);
    this.reservedQuantum = new AtomicInteger(0);
    this.reservedNitrogen = new AtomicInteger(0);
  }

  public void refuel(int id, int nitrogenAmmount, int quantumAmmount) throws InterruptedException {
    // take parking space
    synchronized (parking) {
      while ((occupiedSlots.get() >= slotsCap) || !isFuel(nitrogenAmmount, quantumAmmount)) {
        System.out.println(id + " waiting");
        parking.wait();
      }
      occupiedSlots.getAndIncrement();
      reserveFuel(nitrogenAmmount, quantumAmmount);
      System.out.printf("%d has taken a spot (%d/%d).\n", id, occupiedSlots.get(), slotsCap);
    }
    getFuel(nitrogenAmmount, quantumAmmount);
    releaseSpot(id);
    synchronized (fuelTank) {
      fuelTank.notifyAll();
    }
  }

  public void refillNitrogenTank(int id, int nitrogenRefill) throws InterruptedException {
    synchronized (fuelTank) {
      while (((nitrogenCap - nitrogenLevel.get()) < nitrogenRefill)) {
        fuelTank.wait();
      }
      putFuel(nitrogenRefill, 0);
      System.out.println("tanker %d has refueled nitrogen tank!");
      System.out.printf("nitrogen: %d/%d (+%d), quantum: %d/%d\n", nitrogenLevel.get(), nitrogenCap, nitrogenRefill,
          quantumLevel.get(), quantumCap);
    }
    synchronized (parking) {
      parking.notifyAll();
    }
  }

  public void refillQuantumTank(int id, int quantumRefill) throws InterruptedException {
    synchronized (fuelTank) {
      while (((quantumCap - quantumLevel.get()) < quantumRefill)) {
        fuelTank.wait();
      }
      putFuel(0, quantumRefill);
      System.out.println("tanker %d has refueled quantum tank!");
      System.out.printf("nitrogen: %d/%d, quantum: %d/%d (+%d)\n", nitrogenLevel.get(), nitrogenCap, quantumLevel.get(),
          quantumCap, quantumRefill);
    }
    synchronized (parking) {
      parking.notifyAll();
    }
  }

  private void releaseSpot(int id) {
    System.out.println(id + " releasing a spot.");
    synchronized (parking) {
      parking.notify();
      occupiedSlots.getAndDecrement();
    }
  }

  private void reserveFuel(int nitrogenAmmount, int quantumAmmount) {
    if (nitrogenAmmount > 0)
      this.reservedNitrogen.addAndGet(nitrogenAmmount);
    if (quantumAmmount > 0)
      this.reservedQuantum.addAndGet(quantumAmmount);
  }

  private synchronized void getFuel(int nitrogenAmmount, int quantumAmmount) {
    if (nitrogenAmmount > 0) {
      this.nitrogenLevel.addAndGet(-nitrogenAmmount);
      this.reservedNitrogen.addAndGet(-nitrogenAmmount);
    }
    if (quantumAmmount > 0) {
      this.quantumLevel.addAndGet(-quantumAmmount);
      this.reservedQuantum.addAndGet(-quantumAmmount);
    }
    System.out.printf("nitrogen: %d/%d (-%d), quantum: %d/%d (-%d)\n", nitrogenLevel.get(), nitrogenCap,
        nitrogenAmmount, quantumLevel.get(), quantumCap, quantumAmmount);
    try {
      // simulate refuel time
      Thread.sleep((nitrogenAmmount + quantumAmmount) * 100);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }

  private synchronized void putFuel(int nitrogenRefill, int quantumRefill) {
    if (nitrogenRefill > 0)
      this.nitrogenLevel.addAndGet(nitrogenRefill);
    if (quantumRefill > 0)
      this.quantumLevel.addAndGet(quantumRefill);
    // simulate refuel time
    try {
      Thread.sleep((nitrogenRefill + quantumRefill) * 100);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
  }

  private synchronized boolean isFuel(int nitrogenAmmount, int quantumAmmount) {
    boolean status = true;
    if ((this.nitrogenLevel.get() - this.reservedNitrogen.get()) < nitrogenAmmount)
      status = false;
    if ((this.quantumLevel.get() - this.reservedQuantum.get()) < quantumAmmount)
      status = false;
    return status;
  }
}