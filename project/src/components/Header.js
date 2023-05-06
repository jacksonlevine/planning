export default function Header() {
    let imgStyle = {
        width: "60px",
        height: "auto",

    }
    let headStyle = {
        color: "white",
        alignItems: "center",
        display: "flex",
        flexDirection: "row",
        justifyContent: "center",
        margin: "0 0"
    }
    return (
        <h1 style = {headStyle}><img style={imgStyle} src="logo192.png" alt=""/>MimosDono</h1>
    );
}