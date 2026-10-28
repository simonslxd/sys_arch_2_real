const lab = {
  utstyrsliste: [
    { utstyr_navn: "Digitalt multimeter", antall_totalt: 3, antall_tilgjengelig: 3 },
    { utstyr_navn: "Laboratorievekt", antall_totalt: 2, antall_tilgjengelig: 2 },
    { utstyr_navn: "Temperatursensor", antall_totalt: 4, antall_tilgjengelig: 4 },
  ],
  student_liste: [
    { studentnummer: "DEMO-001", navn: "Student Eksempel A", mine_laan: [] },
    { studentnummer: "DEMO-002", navn: "Student Eksempel B", mine_laan: [] },
    { studentnummer: "DEMO-003", navn: "Student Eksempel C", mine_laan: [] },
  ],
  utlaansliste: [],
};

const equipmentList = document.querySelector("#equipment-list");
const studentList = document.querySelector("#student-list");
const activeLoanList = document.querySelector("#active-loan-list");
const studentSelect = document.querySelector("#loan-student");
const equipmentSelect = document.querySelector("#loan-equipment");
const returnSelect = document.querySelector("#return-loan");
const returnButton = document.querySelector("#return-form button");
const feedback = document.querySelector("#feedback");

function finnObjekt(utstyr_navn) {
  return lab.utstyrsliste.find((utstyr) => utstyr.utstyr_navn === utstyr_navn);
}

function er_tilgjengelig(utstyr) {
  return utstyr.antall_tilgjengelig > 0;
}

function oek_antall_tilgjengelig(utstyr) {
  utstyr.antall_tilgjengelig += 1;
}

function reduser_antall_tilgjengelig(utstyr) {
  utstyr.antall_tilgjengelig -= 1;
}

function regUtlaan(utstyr) {
  reduser_antall_tilgjengelig(utstyr);
}

function regRetur(utstyr) {
  oek_antall_tilgjengelig(utstyr);
}

function finn_tilgjengelig_utstyr(utstyr) {
  return er_tilgjengelig(utstyr);
}

function finnStudent(studentnummer) {
  return lab.student_liste.find((student) => student.studentnummer === studentnummer);
}

function legg_til_laan(student, utlaan) {
  student.mine_laan.push(utlaan);
}

function fjern_laan(student, utlaan) {
  const loanIndex = student.mine_laan.indexOf(utlaan);

  if (loanIndex === -1) {
    throw new Error("Utlaan finnes ikke i Student.mine_laan.");
  }

  student.mine_laan.splice(loanIndex, 1);
}

function settStudentPaaLaan(utstyr, student) {
  const utlaan = {
    laanetaker: student,
    utlaant_utstyr: utstyr,
    antall: 1,
  };

  regUtlaan(utstyr);
  lab.utlaansliste.push(utlaan);
  legg_til_laan(student, utlaan);
  return utlaan;
}

function laan_ut(studentnummer, utstyr_navn) {
  const utstyr = finnObjekt(utstyr_navn);

  if (!utstyr) {
    throw new Error("Valgt Lab_Utstyr finnes ikke i eksempeldataene.");
  }

  if (!finn_tilgjengelig_utstyr(utstyr)) {
    return { status: "unavailable" };
  }

  const student = finnStudent(studentnummer);

  if (!student) {
    throw new Error("Valgt Student finnes ikke i eksempeldataene.");
  }

  return { status: "created", utlaan: settStudentPaaLaan(utstyr, student) };
}

function registrer_retur(utlaan) {
  const loanIndex = lab.utlaansliste.indexOf(utlaan);

  if (loanIndex === -1) {
    throw new Error("Utlaan finnes ikke lenger i den aktive listen.");
  }

  fjern_laan(utlaan.laanetaker, utlaan);
  for (let unit = 0; unit < utlaan.antall; unit += 1) {
    regRetur(utlaan.utlaant_utstyr);
  }
  lab.utlaansliste.splice(loanIndex, 1);
}

function createOption(value, label) {
  const option = document.createElement("option");
  option.value = value;
  option.textContent = label;
  return option;
}

function renderEquipment() {
  equipmentList.replaceChildren();

  lab.utstyrsliste.forEach((utstyr) => {
    const row = document.createElement("tr");
    const name = document.createElement("td");
    const available = document.createElement("td");
    const total = document.createElement("td");
    const statusCell = document.createElement("td");
    const status = document.createElement("span");

    name.textContent = utstyr.utstyr_navn;
    available.textContent = utstyr.antall_tilgjengelig;
    total.textContent = utstyr.antall_totalt;
    status.className = `status-badge ${
      er_tilgjengelig(utstyr) ? "status-available" : "status-loaned"
    }`;
    status.textContent = er_tilgjengelig(utstyr) ? "LEDIG" : "UTLÅNT";
    statusCell.append(status);
    row.append(name, available, total, statusCell);
    equipmentList.append(row);
  });

  document.querySelector("#equipment-count").textContent =
    `${lab.utstyrsliste.length} typer`;
}

function renderStudents() {
  studentList.replaceChildren();

  lab.student_liste.forEach((student) => {
    const item = document.createElement("li");
    const name = document.createElement("span");
    const number = document.createElement("span");

    item.className = "record-item";
    name.className = "record-primary";
    name.textContent = student.navn;
    number.className = "record-secondary";
    number.textContent = student.studentnummer;
    name.append(number);
    item.append(name);
    studentList.append(item);
  });

  document.querySelector("#student-count").textContent =
    `${lab.student_liste.length} studenter`;
}

function renderLoans() {
  activeLoanList.replaceChildren();
  returnSelect.replaceChildren();

  lab.utlaansliste.forEach((utlaan, index) => {
    const item = document.createElement("li");
    const details = document.createElement("span");
    const amount = document.createElement("span");
    const borrowerName = utlaan.laanetaker.navn;
    const equipmentName = utlaan.utlaant_utstyr.utstyr_navn;

    item.className = "record-item";
    details.className = "record-primary";
    details.textContent = `${equipmentName}`;

    const borrower = document.createElement("span");
    borrower.className = "record-secondary";
    borrower.textContent = `${borrowerName} · ${utlaan.laanetaker.studentnummer}`;
    details.append(borrower);

    amount.className = "count-badge";
    amount.textContent = `antall: ${utlaan.antall}`;
    item.append(details, amount);
    activeLoanList.append(item);

    returnSelect.append(
      createOption(
        String(index),
        `${equipmentName} — ${borrowerName} (antall: ${utlaan.antall})`,
      ),
    );
  });

  if (lab.utlaansliste.length === 0) {
    returnSelect.append(createOption("", "Ingen aktive utlån"));
  }

  returnSelect.disabled = lab.utlaansliste.length === 0;
  returnButton.disabled = lab.utlaansliste.length === 0;
  document.querySelector("#loan-count").textContent =
    `${lab.utlaansliste.length} aktive`;
}

function renderLoanOptions() {
  studentSelect.replaceChildren();
  lab.student_liste.forEach((student) => {
    studentSelect.append(
      createOption(student.studentnummer, `${student.navn} (${student.studentnummer})`),
    );
  });

  equipmentSelect.replaceChildren();
  if (lab.utstyrsliste.length === 0) {
    equipmentSelect.append(createOption("", "Ingen tilgjengelige eksemplarer"));
    equipmentSelect.disabled = true;
    document.querySelector("#loan-form button").disabled = true;
    return;
  }

  lab.utstyrsliste.forEach((utstyr) => {
    equipmentSelect.append(
      createOption(
        utstyr.utstyr_navn,
        `${utstyr.utstyr_navn} (${utstyr.antall_tilgjengelig} tilgjengelig)`,
      ),
    );
  });
  equipmentSelect.disabled = false;
  document.querySelector("#loan-form button").disabled = false;
}

function render() {
  renderEquipment();
  renderStudents();
  renderLoans();
  renderLoanOptions();
}

document.querySelector("#loan-form").addEventListener("submit", (event) => {
  event.preventDefault();
  const form = new FormData(event.currentTarget);
  const result = laan_ut(form.get("student"), form.get("equipment"));

  if (result.status === "unavailable") {
    feedback.textContent = "Dette utstyret er ikke tilgjengelig for utlån.";
    return;
  }

  feedback.textContent =
    `${result.utlaan.utlaant_utstyr.utstyr_navn} er registrert utlånt til ` +
    `${result.utlaan.laanetaker.navn}. Tilstand: LEDIG → UTLÅNT (regUtlaan()).`;
  render();
});

document.querySelector("#return-form").addEventListener("submit", (event) => {
  event.preventDefault();
  const selectedLoan = lab.utlaansliste[Number(returnSelect.value)];

  if (!selectedLoan) {
    feedback.textContent = "Velg et aktivt Utlaan før du registrerer retur.";
    return;
  }

  const equipmentName = selectedLoan.utlaant_utstyr.utstyr_navn;
  registrer_retur(selectedLoan);
  feedback.textContent =
    `Retur av ${equipmentName} er registrert. Tilstand: UTLÅNT → LEDIG (regRetur()).`;
  render();
});

render();
