import PayPalDonate from "./PayPalDonate.js";

export default function Footer() {

  let footerStyle = {
      alignItems: "center",
      display: "flex",
      flexDirection: "column",
      justifyContent: "center",
      margin: "0 0",
      transform: "translateY(700%)"
  }
  return (
    <div className="payPal" style={footerStyle}
    >
        <PayPalDonate />
        <p>
          <span className="small">Donations are hugely appreciated!</span>
        </p>
    </div>
  );
}