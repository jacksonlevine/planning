export default function Inventory(props)
{
  const styles = {
    position: "absolute",
    top: "85%",
    left: "85%",
    zIndex: "10"
  }
  return (
    <div style={styles}>
      <p>{props.id}</p>
    </div>
  );
}