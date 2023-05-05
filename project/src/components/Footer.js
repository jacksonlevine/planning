import PayPalDonate from "./PayPalDonate.js";

export default function Footer() {

  let footerStyle = {
      alignItems: "center",
      display: "flex",
      flexDirection: "row",
      justifyContent: "flex-start",
      margin: "0 0",
      position: "absolute",
      top: "90%",
      left: "0%"
  }
  return (
    <div>
        <p>
          <PayPalDonate />
          <span className="small">Donations are hugely appreciated!</span>
        </p>
    </div>
  );
}